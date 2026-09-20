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

    // Use actual drift from history: validate the PEAK frequency against the
    // previous peak (stored by update_statistics / apply_sweep_tracking), NOT
    // the slice center. The old call passed center_freq here, so every sweep
    // step (up to 8.8 MHz retune) looked like a giant drift and the stability
    // feature collapsed — the gate either always-fired or (with the old
    // unreachable threshold scale) never-fired.
    FeatureVector sample = extract_features(rssi, frequency, frequency, stats.last_tuned_frequency);
    int32_t distance_sq_x10 = compute_distance_squared(sample, stats);

    // Threshold is D² ×10 in raw feature units (compute_distance_squared
    // returns D²×10). threshold_x10 is the user D² setting ×10, so the
    // comparison needs no rescale: thr=40 (D²=4.0) rejects samples with
    // D² > 4.0. Old code multiplied by Q_SCALE (×256) — at default 40 the
    // gate needed D² > 1024 of max ~512, i.e. it could NEVER reject.
    const int32_t threshold = static_cast<int32_t>(threshold_x10);

    return distance_sq_x10 < threshold;
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
    } else {
        stats.sample_count = MAHALANOBIS_HISTORY_SIZE;
    }

    // EXACT recompute from the 8-sample ring (replaces the broken incremental
    // Welford: the old `variance += (delta*delta2)/(n-1)` ADDED the per-sample
    // term to the accumulator instead of tracking M2, so variance grew without
    // bound and D² decayed to 0 for long-tracked drones — the gate slowly
    // became pass-through. n <= 8: 16 MACs, ~40 cycles, no drift.
    // Stack: ~16B. No division by zero (n >= 1 guarded below).
    const uint8_t n = (stats.sample_count < MAHALANOBIS_HISTORY_SIZE) ? stats.sample_count : MAHALANOBIS_HISTORY_SIZE;
    for (uint8_t i = 0; i < MAHALANOBIS_DIMENSIONS; ++i) {
        int32_t sum = 0;
        for (uint8_t s = 0; s < n; ++s) sum += stats.history[s][i];
        const int16_t mean = (n > 0) ? static_cast<int16_t>(sum / n) : sample[i];
        stats.mean[i] = mean;
        int32_t var = 0;
        for (uint8_t s = 0; s < n; ++s) {
            const int32_t d = static_cast<int32_t>(stats.history[s][i]) - mean;
            var += d * d;
        }
        // variance[] is int16_t (struct layout fixed for BSS budget): clamp
        // the pathological alternating-extremes case (~75k) to 32767 instead
        // of wrapping negative (which would invert the distance term).
        int32_t v = (n > 1) ? (var / (n - 1)) : 0;
        if (v > 32767) v = 32767;
        stats.variance[i] = static_cast<int16_t>(v);
    }

    // Store current PEAK frequency for next drift measurement (validate()
    // compares peak-vs-previous-peak; center_freq is kept for API compat).
    stats.last_tuned_frequency = tuned_freq;
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
    rssi_norm = (rssi_norm * 256) / (MAHALANOBIS_RSSI_MAX_DBM - MAHALANOBIS_RSSI_MIN_DBM);
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

    // Convert drift to stability metric (raw 0..256 units, NOT Q8.8 — the
    // statistics layer works in raw units since the recompute fix).
    // Stability = 256 when drift = 0 (no frequency change); decreases
    // linearly with drift, 0 when drift >= FREQUENCY_BANDWIDTH_HZ.
    int32_t stability = 256;
    if (abs_diff < FREQUENCY_BANDWIDTH_HZ) {
        stability = static_cast<int32_t>(
            ((FREQUENCY_BANDWIDTH_HZ - abs_diff) * 256) / FREQUENCY_BANDWIDTH_HZ);
    } else {
        stability = 0;
    }
    features[1] = static_cast<int16_t>(stability);

    return features;
}

int32_t MahalanobisDetector::compute_distance_squared(
    const FeatureVector& sample,
    const MahalanobisStatistics& stats
) const noexcept {
    // Diagonal-covariance Mahalanobis distance in RAW feature units:
    //   D² = Σ (diff² / var), returned ×10 for the threshold compare.
    // diff ∈ [-256, 256] → diff² ≤ 65536 → ×10 = 655360 (fits int32).
    // var floored at 64 (std ≈ 8 raw units ≈ 0.6 dB RSSI / 3% stability):
    // tight enough to reject real outliers, loose enough that Q-noise
    // (±1 unit from integer truncation in extract_features) never fires it.
    // Old code mixed Q8.8 scaling (diff²/Q then ×Q/var with var clamped to
    // 256..32767): a 32-unit RSSI jump scored D² ≈ 8 — invisible against the
    // old threshold (≈1024 at default 40). Now it scores ≈160 vs thr 40.
    int32_t distance_x10 = 0;

    for (uint8_t i = 0; i < MAHALANOBIS_DIMENSIONS; ++i) {
        const int32_t diff = static_cast<int32_t>(sample[i]) - stats.mean[i];
        int32_t var = stats.variance[i];
        if (var < 64) {
            var = 64;
        }

        const int32_t diff_sq = diff * diff;  // ≤ 65536, no overflow
        distance_x10 += (diff_sq * 10) / var;
    }

    return distance_x10;
}

} // namespace drone_analyzer
