#include "pattern_types.hpp"

namespace drone_analyzer {

SignalPattern::SignalPattern() noexcept
    : waveform{}
    , features{}
    , match_threshold(DEFAULT_PATTERN_CORRELATION_THRESHOLD)
    , flags(SignalPattern::Flags::ENABLED)
    , created_time(0)
    , match_count(0) {
    memset(name, 0, PATTERN_NAME_MAX_LEN);
}

} // namespace drone_analyzer
