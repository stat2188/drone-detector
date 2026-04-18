#include "pattern_types.hpp"

namespace drone_analyzer {

SignalPattern::SignalPattern() noexcept
    : name{}                     // Zero-initialize char array (C++11 aggregate init)
    , waveform{}                  // Zero-initialize uint8_t array (C++11 aggregate init)
    , features{}                   // PatternFeatures has member initializer list in struct
    , match_threshold(DEFAULT_PATTERN_CORRELATION_THRESHOLD)
    , flags(SignalPattern::Flags::ENABLED)
    , created_time(0)
    , match_count(0) {
}

} // namespace drone_analyzer
