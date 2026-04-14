/*
 * Copyright (C) 2025 Enhanced Drone Analyzer
 *
 * Mahalanobis Gate Filter Implementation
 *
 * All arithmetic is integer-only using Q8.8 fixed-point.
 * No heap allocation, no exceptions, no floating-point.
 */

#include "mahalanobis_gate.hpp"
#include "constants.hpp"

namespace drone_analyzer {

// ============================================================================
// Public Methods
// ============================================================================

bool MahalanobisDetector::validate(
    RssiValue rssi,
    FreqHz frequency,
    const MahalanobisStatistics& stats,
    uint8_t threshold_x10
) const noexcept {
    // Require at least half of history size for statistical confidence
    constexpr uint8_t MIN_SAMPLES = MAHALANOBIS_HISTORY_SIZE / 2;  // 4 samples
    if (stats.sample_count < MIN_SAMPLES) {
        return true;
    }

    if (threshold_x10 == 0) {
        return true;
    }

    // Use actual drift from history instead of degenerate center_freq comparison
    FeatureVector sample = extract_features(rssi, frequency, frequency, stats.last_tuned_frequency);
    int32_t distance_sq = compute_distance_squared(sample, stats);

    int32_t threshold_sq = static_cast<int32_t>(threshold_x10) * threshold_x10;
    threshold_sq = (threshold_sq * Q_SCALE) / 100;

    return distance_sq < threshold_sq;
}

void MahalanobisDetector::update_statistics(
    MahalanobisStatistics& stats,
    RssiValue rssi,
    FreqHz center_freq,
    FreqHz tuned_freq
) noexcept {
    // Extract features using the previous tuned frequency for drift measurement
    const FreqHz last_freq = stats.last_tuned_frequency;
    FeatureVector sample = extract_features(rssi, center_freq, tuned_freq, last_freq);

    stats.history[stats.history_index] = sample;
    stats.history_index = (stats.history_index + 1) % MAHALANOBIS_HISTORY_SIZE;

    if (stats.sample_count < MAHALANOBIS_HISTORY_SIZE) {
        stats.sample_count++;
    }

    for (uint8_t i = 0; i < MAHALANOBIS_DIMENSIONS; ++i) {
        int32_t n = stats.sample_count;

        if (n < 2) {
            stats.mean[i] = sample[i];
            continue;
        }

        int32_t delta = sample[i] - stats.mean[i];
        stats.mean[i] += delta / n;
        int32_t delta2 = sample[i] - stats.mean[i];
        stats.variance[i] += (delta * delta2) / (n - 1);
    }

    // Store current tuned frequency for next drift measurement
    stats.last_tuned_frequency = tuned_freq;

    // FIXED: Aggressive variance decay to prevent unbounded accumulation.
    // Previous logic only decayed when sample_count >= HISTORY_SIZE and history_index == 0,
    // which could happen very rarely or not at all if samples are missed.
    // New logic: decay every 16 samples regardless of history_index, ensuring
    // variance stays bounded and gate remains effective during long scans.
    if (stats.sample_count > 0 && (stats.sample_count % 16) == 0) {
        for (uint8_t i = 0; i < MAHALANOBIS_DIMENSIONS; ++i) {
            stats.variance[i] = (stats.variance[i] * 15) / 16;
            if (stats.variance[i] < MAHALANOBIS_MIN_VARIANCE) {
                stats.variance[i] = MAHALANOBIS_MIN_VARIANCE;
            }
        }
    }
}
}

// ============================================================================
// Private Methods
// ============================================================================

MahalanobisDetector::FeatureVector MahalanobisDetector::extract_features(
    RssiValue rssi,
    FreqHz center_freq,
    FreqHz tuned_freq,
    FreqHz last_tuned_frequency
) const noexcept {
    FeatureVector features{};

    int16_t rssi_clamped = static_cast<int16_t>(
        (rssi < MAHALANOBIS_RSSI_MIN_DBM) ? MAHALANOBIS_RSSI_MIN_DBM :
        (rssi > MAHALANOBIS_RSSI_MAX_DBM) ? MAHALANOBIS_RSSI_MAX_DBM : rssi
    );

    int32_t rssi_norm = rssi_clamped - MAHALANOBIS_RSSI_MIN_DBM;
    rssi_norm = (rssi_norm * 255) / (MAHALANOBIS_RSSI_MAX_DBM - MAHALANOBIS_RSSI_MIN_DBM);
    rssi_norm = q_multiply_safe(rssi_norm, Q_SCALE) / 255;
    features[0] = static_cast<int16_t>(rssi_norm);

    // center_freq parameter is kept for API compatibility but unused for stability calculation
    (void)center_freq;

    // Measure actual drift from previous measurement to fix degenerate feature
    uint64_t abs_diff = 0;
    if (last_tuned_frequency != 0) {
        // Compute absolute difference between current and previous tuned frequencies
        if (tuned_freq >= last_tuned_frequency) {
            abs_diff = tuned_freq - last_tuned_frequency;
        } else {
            abs_diff = last_tuned_frequency - tuned_freq;
        }
    }

    // Convert drift to stability metric (Q8.8)
    // Stability = 256 when drift = 0 (no frequency change)
    // Stability decreases linearly with drift, minimum = 0 when drift >= FREQUENCY_BANDWIDTH_HZ
    int32_t stability = Q_SCALE;
    if (abs_diff < FREQUENCY_BANDWIDTH_HZ) {
        int32_t bandwidth_delta = static_cast<int32_t>(FREQUENCY_BANDWIDTH_HZ - abs_diff);
        stability = q_multiply_safe(bandwidth_delta, Q_SCALE);
        stability = stability / FREQUENCY_BANDWIDTH_HZ;
    }
    features[1] = static_cast<int16_t>(stability);

    return features;
}

int32_t MahalanobisDetector::compute_distance_squared(
    const FeatureVector& sample,
    const MahalanobisStatistics& stats
) const noexcept {
    int32_t distance_sq = 0;

    for (uint8_t i = 0; i < MAHALANOBIS_DIMENSIONS; ++i) {
        int32_t diff_Q = sample[i] - stats.mean[i];

        int64_t diff_sq_64 = static_cast<int64_t>(diff_Q) * diff_Q;
        diff_sq_64 /= Q_SCALE;

        int32_t var = stats.variance[i];
        if (var < MAHALANOBIS_MIN_VARIANCE) {
            var = MAHALANOBIS_MIN_VARIANCE;
        }
        if (var > 32767) {
            var = 32767;
        }

        int32_t diff_sq = static_cast<int32_t>(diff_sq_64);
        int64_t term_64 = static_cast<int64_t>(diff_sq) * Q_SCALE;
        term_64 /= var;

        int32_t term = static_cast<int32_t>(term_64);
        distance_sq += term;
    }

    return distance_sq;
}

} // namespace drone_analyzer
