#ifndef PATTERN_TYPES_HPP
#define PATTERN_TYPES_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "drone_types.hpp"

namespace drone_analyzer {

constexpr size_t MAX_PATTERNS = 20;
constexpr size_t PATTERN_WAVEFORM_SIZE = 16;
constexpr char PATTERN_DIR[] = "/PATTERNS";
constexpr size_t PATTERN_NAME_MAX_LEN = 28;
constexpr uint16_t DEFAULT_PATTERN_CORRELATION_THRESHOLD = 200;
constexpr uint8_t DEFAULT_PATTERN_MATCH_CONFIDENCE = 50;

enum class PatternMatchStatus : uint8_t {
    NO_MATCH = 0,
    WEAK_MATCH = 1,
    MODERATE_MATCH = 2,
    STRONG_MATCH = 3,
    EXCELLENT_MATCH = 4
};

struct PatternFeatures {
    uint8_t peak_position;
    uint8_t peak_value;
    uint8_t noise_floor;
    uint8_t margin;
    uint8_t width;
    uint8_t sharpness;
    uint8_t flatness;
    uint8_t symmetry;

    PatternFeatures() noexcept
        : peak_position(0)
        , peak_value(0)
        , noise_floor(0)
        , margin(0)
        , width(0)
        , sharpness(0)
        , flatness(0)
        , symmetry(0) {}
};

struct SignalPattern {
    char name[PATTERN_NAME_MAX_LEN];
    uint8_t waveform[PATTERN_WAVEFORM_SIZE];
    PatternFeatures features;
    uint8_t match_threshold;
    uint8_t flags;
    uint32_t created_time;
    uint32_t match_count;

    enum Flags : uint8_t {
        NONE = 0x00,
        ENABLED = 0x01,
        LOCKED = 0x02,
        AUTO_THRESHOLD = 0x04
    };

    SignalPattern() noexcept;

    [[nodiscard]] bool is_enabled() const noexcept {
        return (flags & ENABLED) != 0;
    }

    [[nodiscard]] bool is_locked() const noexcept {
        return (flags & LOCKED) != 0;
    }

    void set_enabled(bool enabled) noexcept {
        if (enabled) {
            flags |= ENABLED;
        } else {
            flags &= ~ENABLED;
        }
    }

    [[nodiscard]] bool is_valid() const noexcept {
        if (name[0] == '\0') return false;
        if (match_threshold == 0) return false;
        return true;
    }
};

struct PatternMatchResult {
    size_t pattern_index;
    uint16_t correlation_score;
    uint8_t feature_distance;
    PatternMatchStatus status;
    bool matched;

    PatternMatchResult() noexcept
        : pattern_index(0)
        , correlation_score(0)
        , feature_distance(255)
        , status(PatternMatchStatus::NO_MATCH)
        , matched(false) {}

    static PatternMatchResult no_match() noexcept {
        PatternMatchResult result;
        result.matched = false;
        result.status = PatternMatchStatus::NO_MATCH;
        return result;
    }
};

} // namespace drone_analyzer

#endif // PATTERN_TYPES_HPP
