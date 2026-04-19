#include "pattern_types.hpp"

namespace drone_analyzer {

SignalPattern::SignalPattern() noexcept
    : name{}
    , waveform{}
    , features{}
    , match_threshold(DEFAULT_PATTERN_CORRELATION_THRESHOLD)
    , flags(SignalPattern::Flags::ENABLED)
    , created_time(0)
    , match_count(0) {
}

} // namespace drone_analyzer
