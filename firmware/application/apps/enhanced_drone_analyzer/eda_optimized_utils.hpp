#ifndef EDA_OPTIMIZED_UTILS_HPP_
#define EDA_OPTIMIZED_UTILS_HPP_

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <utility>

#include "eda_constants.hpp"
#include "ui_drone_common_types.hpp"

namespace ui {
namespace apps {
namespace enhanced_drone_analyzer {

using BufferIndex = uint16_t;
using WindowSize = uint16_t;

namespace SNRThresholds {
    constexpr uint8_t MILITARY_CRITICAL = 15;
    constexpr uint8_t MILITARY_HIGH = 10;
    constexpr uint8_t FPV_HIGH = 20;
    constexpr uint8_t FPV_MEDIUM = 10;
    constexpr uint8_t FPV_LOW = 5;
    constexpr uint8_t COMMERCIAL_CRITICAL = 25;
    constexpr uint8_t COMMERCIAL_HIGH = 15;
    constexpr uint8_t COMMERCIAL_MEDIUM = 10;
    constexpr uint8_t COMMERCIAL_LOW = 5;
}

namespace TrendConstants {
    constexpr uint8_t NUM_SYMBOLS = 4;
}

namespace BufferSizes {
    constexpr uint32_t ERROR_BUFFER_SIZE = 128;
    constexpr uint32_t FREQ_BUFFER_SIZE = 32;
}

template<typename T, WindowSize N = 11>
class MedianFilter {
public:
    static_assert(N <= 256, "MedianFilter window size too large for stack (max 256)");

    constexpr MedianFilter() noexcept : window_{}, head_(0), full_(false) {}

    void add(const T value) noexcept {
        window_[head_] = value;
        head_ = (head_ + 1) % N;
        if (head_ == 0) full_ = true;
    }

    constexpr WindowSize get_current_size() const noexcept {
        return full_ ? N : head_;
    }

    T get_median() const noexcept {
        const WindowSize current_size = get_current_size();

        if (current_size == 0) {
            return T{};
        }

        std::array<T, N> temp{};
        for (WindowSize i = 0; i < current_size; ++i) {
            temp[i] = window_[i];
        }

        const WindowSize k = current_size / 2;
        WindowSize left = 0;
        WindowSize right = current_size - 1;

        while (left < right) {
            WindowSize pivot_idx = left + (right - left) / 2;
            T pivot = temp[pivot_idx];

            std::swap(temp[pivot_idx], temp[right]);
            WindowSize store_idx = left;

            for (WindowSize i = left; i < right; ++i) {
                if (temp[i] < pivot) {
                    std::swap(temp[store_idx], temp[i]);
                    store_idx++;
                }
            }

            std::swap(temp[store_idx], temp[right]);

            if (store_idx == k) {
                break;
            } else if (store_idx < k) {
                left = store_idx + 1;
            } else {
                right = store_idx - 1;
            }
        }

        return temp[k];
    }

    void reset() noexcept {
        full_ = false;
        head_ = 0;
        window_.fill(static_cast<T>(0));
    }

private:
    std::array<T, N> window_;
    BufferIndex head_;
    bool full_;
};

struct FrequencyValidator {
    static constexpr bool is_valid_frequency(int64_t hz) noexcept {
        return EDA::Validation::validate_frequency(hz);
    }

    static constexpr bool is_valid_2_4ghz_band(int64_t hz) noexcept {
        return EDA::Validation::is_2_4ghz_band(hz);
    }

    static constexpr bool is_valid_5_8ghz_band(int64_t hz) noexcept {
        return EDA::Validation::is_5_8ghz_band(hz);
    }

    static constexpr bool is_valid_military_band(int64_t hz) noexcept {
        return EDA::Validation::is_military_band(hz);
    }

    static constexpr bool is_valid_433mhz_ism(int64_t hz) noexcept {
        return EDA::Validation::is_433mhz_band(hz);
    }
};

struct DroneTypeDetector {
    static constexpr DroneType from_frequency(int64_t hz) noexcept {
        if (FrequencyValidator::is_valid_2_4ghz_band(hz)) {
            return DroneType::MAVIC;
        }
        if (FrequencyValidator::is_valid_5_8ghz_band(hz)) {
            return DroneType::FPV_RACING;
        }
        if (FrequencyValidator::is_valid_military_band(hz)) {
            return DroneType::MILITARY_DRONE;
        }
        if (FrequencyValidator::is_valid_433mhz_ism(hz)) {
            return DroneType::DIY_DRONE;
        }
        return DroneType::UNKNOWN;
    }
};

struct FrequencyFormatter {
    enum class Format {
        COMPACT_GHZ,
        COMPACT_MHZ,
        STANDARD_GHZ,
        STANDARD_MHZ,
        DETAILED_GHZ,
        SPACED_GHZ
    };

    static void format_to_buffer(char* __restrict__ buffer, uint32_t buffer_size,
                                  int64_t freq_hz, Format fmt) noexcept {
        if (!buffer || buffer_size == 0) return;

        switch (fmt) {
            case Format::COMPACT_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 500000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000LL) / 100000000ULL);
                if (decimal > 0) {
                    std::snprintf(buffer, buffer_size, "%lu.%luG",
                             static_cast<unsigned long>(ghz),
                             static_cast<unsigned long>(decimal));
                } else {
                    std::snprintf(buffer, buffer_size, "%luG",
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::COMPACT_MHZ: {
                uint32_t mhz = static_cast<uint32_t>((freq_hz + 500000) / 1000000LL);
                std::snprintf(buffer, buffer_size, "%lu",
                         static_cast<unsigned long>(mhz));
                break;
            }
            case Format::STANDARD_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 50000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000LL) / 100000000ULL);
                if (decimal > 0) {
                    std::snprintf(buffer, buffer_size, "%lu.%luGHz",
                             static_cast<unsigned long>(ghz),
                             static_cast<unsigned long>(decimal));
                } else {
                    std::snprintf(buffer, buffer_size, "%luGHz",
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::STANDARD_MHZ: {
                uint32_t mhz = static_cast<uint32_t>(freq_hz / 1000000LL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000LL) / 100000ULL);
                if (decimals > 0) {
                    std::snprintf(buffer, buffer_size, "%lu.%luMHz",
                             static_cast<unsigned long>(mhz),
                             static_cast<unsigned long>(decimals));
                } else {
                    std::snprintf(buffer, buffer_size, "%luMHz",
                             static_cast<unsigned long>(mhz));
                }
                break;
            }
            case Format::DETAILED_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 5000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000000LL) / 10000000ULL);
                std::snprintf(buffer, buffer_size, "%lu.%03luGHz",
                         static_cast<unsigned long>(ghz),
                         static_cast<unsigned long>(decimals));
                break;
            }
            case Format::SPACED_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 500000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                std::snprintf(buffer, buffer_size, "%lu GHz",
                         static_cast<unsigned long>(ghz));
                break;
            }
        }
    }

    static void to_string_short_freq_buffer(char* __restrict__ buffer, uint32_t buffer_size,
                                              int64_t freq_hz) noexcept {
        format_to_buffer(buffer, buffer_size, freq_hz, Format::COMPACT_GHZ);
    }
};

struct ThreatClassifier {
    static constexpr ThreatLevel from_rssi(int32_t rssi_db) noexcept {
        if (rssi_db >= EDA::Constants::CRITICAL_RSSI_DB) return ThreatLevel::CRITICAL;
        if (rssi_db >= EDA::Constants::HIGH_RSSI_DB) return ThreatLevel::HIGH;
        if (rssi_db >= EDA::Constants::MEDIUM_RSSI_DB) return ThreatLevel::MEDIUM;
        if (rssi_db >= EDA::Constants::LOW_RSSI_DB) return ThreatLevel::LOW;
        return ThreatLevel::NONE;
    }

    static constexpr ThreatLevel from_snr_and_type(uint8_t snr, uint8_t type) noexcept {
        if (type == static_cast<uint8_t>(DroneType::MILITARY_DRONE)) {
            if (snr >= SNRThresholds::MILITARY_CRITICAL) return ThreatLevel::CRITICAL;
            if (snr >= SNRThresholds::MILITARY_HIGH) return ThreatLevel::HIGH;
            return ThreatLevel::MEDIUM;
        }

        if (type == static_cast<uint8_t>(DroneType::FPV_RACING)) {
            if (snr >= SNRThresholds::FPV_HIGH) return ThreatLevel::HIGH;
            if (snr >= SNRThresholds::FPV_MEDIUM) return ThreatLevel::MEDIUM;
            if (snr >= SNRThresholds::FPV_LOW) return ThreatLevel::LOW;
            return ThreatLevel::NONE;
        }

        if (snr >= SNRThresholds::COMMERCIAL_CRITICAL) return ThreatLevel::CRITICAL;
        if (snr >= SNRThresholds::COMMERCIAL_HIGH) return ThreatLevel::HIGH;
        if (snr >= SNRThresholds::COMMERCIAL_MEDIUM) return ThreatLevel::MEDIUM;
        if (snr >= SNRThresholds::COMMERCIAL_LOW) return ThreatLevel::LOW;
        return ThreatLevel::NONE;
    }
};

struct TrendSymbols {
    static constexpr char SYMBOLS[TrendConstants::NUM_SYMBOLS] = {
        '=',
        '^',
        'v',
        '~'
    };

    static constexpr char from_trend(uint8_t trend) noexcept {
        return (trend < TrendConstants::NUM_SYMBOLS) ? SYMBOLS[trend] : SYMBOLS[3];
    }
};

struct StatusFormatter {
    template<uint32_t N, typename... Args>
    static void format_to(char (&buffer)[N], const char* fmt, Args&&... args) noexcept {
        std::snprintf(buffer, N, fmt, std::forward<Args>(args)...);
    }
};

}
}
}

#endif
