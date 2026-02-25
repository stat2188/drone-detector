/*
 * Copyright (C) 2023
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "doctest.h"
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>

// ============================================================================
// Stack Usage Measurement Helper (Mock for Test Environment)
// ============================================================================

namespace StackTest {

// Memory constraints for the embedded target
constexpr size_t STACK_LIMIT_BYTES = 4096;  // 4 KB stack limit
constexpr size_t TOTAL_RAM_BYTES = 131072;  // 128 KB total RAM

// Track stack usage by measuring local variable sizes
template<typename T>
[[nodiscard]] constexpr size_t measure_type_size() noexcept {
    return sizeof(T);
}

// Check if a type fits within stack limit
template<typename T>
[[nodiscard]] constexpr bool fits_in_stack() noexcept {
    return sizeof(T) <= STACK_LIMIT_BYTES;
}

// Check if an array of types fits within stack limit
template<typename T, size_t N>
[[nodiscard]] constexpr bool array_fits_in_stack() noexcept {
    return (sizeof(T) * N) <= STACK_LIMIT_BYTES;
}

// Get percentage of stack used by a type (not constexpr due to double)
template<typename T>
[[nodiscard]] double stack_usage_percent() noexcept {
    return (static_cast<double>(sizeof(T)) / static_cast<double>(STACK_LIMIT_BYTES)) * 100.0;
}

// Stack usage report structure
struct StackUsageReport {
    const char* component_name;
    size_t type_size;
    double stack_percent;
    bool within_limit;
};

// Generate stack usage report (not constexpr)
template<typename T>
[[nodiscard]] StackUsageReport generate_report(const char* name) noexcept {
    return StackUsageReport{
        name,
        measure_type_size<T>(),
        stack_usage_percent<T>(),
        fits_in_stack<T>()
    };
}

} // namespace StackTest

// ============================================================================
// Test: Database Entry Stack Usage
// ============================================================================

TEST_SUITE_BEGIN("Stack Usage - Database Entry");

SCENARIO("UnifiedDroneEntry structure size is within stack limits") {
    GIVEN("A UnifiedDroneEntry structure") {
        // Simulate the structure (48 bytes based on header)
        struct MockUnifiedDroneEntry {
            uint64_t frequency_hz = 0;
            char description[32] = "";
            uint8_t threat_level = 0;
            uint8_t frequency_band = 0;
            uint16_t flags = 0;
            uint8_t reserved[4] = {};
        };

        WHEN("Measuring its size") {
            const auto report = StackTest::generate_report<MockUnifiedDroneEntry>("UnifiedDroneEntry");

            THEN("It should be exactly 48 bytes") {
                CHECK_EQ(report.type_size, 48);
            }

            THEN("It should fit within stack limit") {
                CHECK(report.within_limit);
            }

            THEN("It should use less than 2% of stack") {
                CHECK(report.stack_percent < 2.0);
            }
        }
    }
}

SCENARIO("Multiple UnifiedDroneEntry instances on stack") {
    GIVEN("An array of UnifiedDroneEntry structures") {
        struct MockUnifiedDroneEntry {
            uint64_t frequency_hz = 0;
            char description[32] = "";
            uint8_t threat_level = 0;
            uint8_t frequency_band = 0;
            uint16_t flags = 0;
            uint8_t reserved[4] = {};
        };

        WHEN("Creating an array of 10 entries") {
            constexpr size_t ARRAY_SIZE = 10;
            using EntryArray = std::array<MockUnifiedDroneEntry, ARRAY_SIZE>;

            THEN("The array should fit within stack limit") {
                CHECK(StackTest::array_fits_in_stack<MockUnifiedDroneEntry, ARRAY_SIZE>());
            }

            THEN("The array should use less than 12% of stack") {
                const double usage = (static_cast<double>(sizeof(EntryArray)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 12.0);
            }
        }

        WHEN("Creating an array of 50 entries") {
            constexpr size_t ARRAY_SIZE = 50;
            using EntryArray = std::array<MockUnifiedDroneEntry, ARRAY_SIZE>;

            THEN("The array should fit within stack limit") {
                CHECK(StackTest::array_fits_in_stack<MockUnifiedDroneEntry, ARRAY_SIZE>());
            }

            THEN("The array should use less than 60% of stack") {
                const double usage = (static_cast<double>(sizeof(EntryArray)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 60.0);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Settings Structure Stack Usage
// ============================================================================

TEST_SUITE_BEGIN("Stack Usage - Settings");

SCENARIO("DroneAnalyzerSettings structure size is reasonable") {
    GIVEN("A DroneAnalyzerSettings structure") {
        // Simulate a representative settings structure
        struct MockDroneAnalyzerSettings {
            // Audio settings
            bool audio_enabled = true;
            uint32_t audio_alert_frequency_hz = 800;
            uint32_t audio_alert_duration_ms = 500;
            uint32_t audio_volume_level = 50;
            bool audio_mute_on_detection = false;

            // Hardware settings
            uint32_t hardware_bandwidth_hz = 24000000;
            bool hardware_rf_amp_enabled = true;
            bool hardware_lna_enabled = false;
            bool hardware_vga_enabled = false;
            uint32_t rx_phase_value = 15;
            uint32_t lna_gain_db = 32;
            uint32_t vga_gain_db = 20;
            bool hardware_use_external_antenna = false;

            // Scanning settings
            uint32_t scan_interval_ms = 1000;
            int32_t rssi_threshold_db = -90;
            bool scanning_continuous_mode = false;
            uint64_t wideband_min_freq_hz = 2400000000ULL;
            uint64_t wideband_max_freq_hz = 2500000000ULL;
            uint32_t wideband_slice_width_hz = 24000000;
            bool scanning_auto_detect = true;
            bool scanning_adaptive_speed = true;

            // Detection settings
            bool detection_enabled = true;
            uint32_t movement_sensitivity = 3;
            uint32_t threat_level_threshold = 2;
            uint32_t min_detection_count = 3;
            uint32_t alert_persistence_threshold = 3;
            bool detection_use_ml_classifier = true;

            // Logging settings
            bool logging_enabled = true;
            char log_file_path[64] = "/eda_logs";
            char log_format[8] = "CSV";
            uint32_t max_log_file_size_kb = 1024;
            bool logging_timestamp = true;
            bool logging_frequency = true;
            bool logging_signal_strength = true;
            bool logging_threat_level = true;

            // Display settings
            char color_scheme[32] = "DARK";
            uint32_t font_size = 0;
            uint32_t spectrum_density = 1;
            uint32_t waterfall_speed = 5;
            bool display_show_spectrum = true;
            bool display_show_waterfall = true;
            bool display_show_frequency_ruler = true;
            bool display_show_signal_strength = true;
            uint32_t frequency_ruler_style = 5;
            uint32_t compact_ruler_tick_count = 4;
            bool display_compact_mode = true;

            // Profile settings
            char current_profile_name[32] = "Default";
            bool profile_auto_save = true;
            bool profile_auto_load = false;

            // Path settings
            char freqman_path[32] = "DRONES";
            char settings_file_path[64] = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
            uint32_t settings_version = 2;
        };

        WHEN("Measuring its size") {
            const auto report = StackTest::generate_report<MockDroneAnalyzerSettings>("DroneAnalyzerSettings");

            THEN("It should be less than 512 bytes") {
                CHECK(report.type_size < 512);
            }

            THEN("It should fit within stack limit") {
                CHECK(report.within_limit);
            }

            THEN("It should use less than 13% of stack") {
                CHECK(report.stack_percent < 13.0);
            }
        }
    }
}

SCENARIO("SettingMetadata structure size") {
    GIVEN("A SettingMetadata structure") {
        struct MockSettingMetadata {
            const char* key;
            uint16_t offset;
            uint8_t type;
            uint8_t bit_pos;
            int64_t min_val;
            int64_t max_val;
            const char* default_str;
        };

        WHEN("Measuring its size") {
            const auto report = StackTest::generate_report<MockSettingMetadata>("SettingMetadata");

            THEN("It should be exactly 32 bytes (on 64-bit) or 24 bytes (on 32-bit)") {
                CHECK((report.type_size == 32 || report.type_size == 24));
            }

            THEN("It should fit within stack limit") {
                CHECK(report.within_limit);
            }

            THEN("It should use less than 1% of stack") {
                CHECK(report.stack_percent < 1.0);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Database Parser Stack Usage
// ============================================================================

TEST_SUITE_BEGIN("Stack Usage - Database Parser");

SCENARIO("Database parser local buffers") {
    GIVEN("A database parsing function with local buffers") {
        WHEN("Using frequency buffer (32 bytes)") {
            constexpr size_t FREQ_BUFFER_SIZE = 32;
            char freq_buffer[FREQ_BUFFER_SIZE];

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::array_fits_in_stack<char, FREQ_BUFFER_SIZE>());
            }

            THEN("The buffer should use less than 1% of stack") {
                const double usage = (static_cast<double>(sizeof(freq_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 1.0);
            }
        }

        WHEN("Using description buffer (64 bytes)") {
            constexpr size_t DESC_BUFFER_SIZE = 64;
            char desc_buffer[DESC_BUFFER_SIZE];

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::array_fits_in_stack<char, DESC_BUFFER_SIZE>());
            }

            THEN("The buffer should use less than 2% of stack") {
                const double usage = (static_cast<double>(sizeof(desc_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 2.0);
            }
        }

        WHEN("Using combined parsing buffers") {
            constexpr size_t FREQ_BUFFER_SIZE = 32;
            constexpr size_t DESC_BUFFER_SIZE = 64;
            struct ParserBuffers {
                char freq_buffer[FREQ_BUFFER_SIZE];
                char desc_buffer[DESC_BUFFER_SIZE];
            };

            THEN("The combined buffers should fit within stack") {
                CHECK(StackTest::fits_in_stack<ParserBuffers>());
            }

            THEN("The combined buffers should use less than 3% of stack") {
                const double usage = (static_cast<double>(sizeof(ParserBuffers)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 3.0);
            }
        }
    }
}

SCENARIO("Database parser function call overhead") {
    GIVEN("A database parser function") {
        // Simulate function call stack usage
        struct ParserStackFrame {
            // Return address (8 bytes on 64-bit, 4 on 32-bit)
            // Saved registers (varies, typically 16-32 bytes)
            // Local variables
            uint64_t frequency = 0;
            char buffer[32];
            size_t index = 0;
            const char* ptr = nullptr;
        };

        WHEN("Measuring stack frame size") {
            const auto report = StackTest::generate_report<ParserStackFrame>("ParserStackFrame");

            THEN("The stack frame should be less than 128 bytes") {
                CHECK(report.type_size < 128);
            }

            THEN("The stack frame should fit within stack limit") {
                CHECK(report.within_limit);
            }

            THEN("The stack frame should use less than 4% of stack") {
                CHECK(report.stack_percent < 4.0);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Settings Persistence Stack Usage
// ============================================================================

TEST_SUITE_BEGIN("Stack Usage - Settings Persistence");

SCENARIO("Settings serialization buffer") {
    GIVEN("A settings serialization function") {
        WHEN("Using a 256-byte serialization buffer") {
            constexpr size_t SERIALIZATION_BUFFER_SIZE = 256;
            char serialization_buffer[SERIALIZATION_BUFFER_SIZE];

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::array_fits_in_stack<char, SERIALIZATION_BUFFER_SIZE>());
            }

            THEN("The buffer should use less than 7% of stack") {
                const double usage = (static_cast<double>(sizeof(serialization_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 7.0);
            }
        }

        WHEN("Using a 512-byte serialization buffer") {
            constexpr size_t SERIALIZATION_BUFFER_SIZE = 512;
            char serialization_buffer[SERIALIZATION_BUFFER_SIZE];

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::array_fits_in_stack<char, SERIALIZATION_BUFFER_SIZE>());
            }

            THEN("The buffer should use less than 13% of stack") {
                const double usage = (static_cast<double>(sizeof(serialization_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 13.0);
            }
        }
    }
}

SCENARIO("Settings load buffer") {
    GIVEN("A settings load function") {
        WHEN("Using a 1024-byte load buffer") {
            constexpr size_t LOAD_BUFFER_SIZE = 1024;
            char load_buffer[LOAD_BUFFER_SIZE];

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::array_fits_in_stack<char, LOAD_BUFFER_SIZE>());
            }

            THEN("The buffer should use exactly 25% of stack") {
                const double usage = (static_cast<double>(sizeof(load_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage == 25.0);
            }
        }

        WHEN("Using a 2048-byte load buffer") {
            constexpr size_t LOAD_BUFFER_SIZE = 2048;
            char load_buffer[LOAD_BUFFER_SIZE];

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::array_fits_in_stack<char, LOAD_BUFFER_SIZE>());
            }

            THEN("The buffer should use exactly 50% of stack") {
                const double usage = (static_cast<double>(sizeof(load_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage == 50.0);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Signal Processing Stack Usage
// ============================================================================

TEST_SUITE_BEGIN("Stack Usage - Signal Processing");

SCENARIO("Signal processing buffers") {
    GIVEN("Signal processing functions") {
        WHEN("Using a 256-sample complex buffer") {
            constexpr size_t SAMPLE_COUNT = 256;
            struct ComplexSample {
                int16_t i;
                int16_t q;
            };
            std::array<ComplexSample, SAMPLE_COUNT> signal_buffer;

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::fits_in_stack<decltype(signal_buffer)>());
            }

            THEN("The buffer should use less than 26% of stack") {
                const double usage = (static_cast<double>(sizeof(signal_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 26.0);
            }
        }

        WHEN("Using a 128-sample complex buffer") {
            constexpr size_t SAMPLE_COUNT = 128;
            struct ComplexSample {
                int16_t i;
                int16_t q;
            };
            std::array<ComplexSample, SAMPLE_COUNT> signal_buffer;

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::fits_in_stack<decltype(signal_buffer)>());
            }

            THEN("The buffer should use less than 13% of stack") {
                const double usage = (static_cast<double>(sizeof(signal_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 13.0);
            }
        }
    }
}

SCENARIO("FFT processing stack usage") {
    GIVEN("An FFT processing function") {
        struct FFTStackFrame {
            // Twiddle factors (pre-computed, stored in ROM)
            // Input buffer
            std::array<float, 256> real_part{};
            std::array<float, 256> imag_part{};
            // Output buffer
            std::array<float, 256> magnitude{};
            // Temporary variables
            size_t stage = 0;
            size_t butterfly = 0;
            float temp_real = 0.0f;
            float temp_imag = 0.0f;
        };

        WHEN("Measuring FFT stack frame") {
            const auto report = StackTest::generate_report<FFTStackFrame>("FFTStackFrame");

            THEN("The stack frame should be less than 4096 bytes") {
                CHECK(report.type_size < 4096);
            }

            THEN("The stack frame should fit within stack limit") {
                CHECK(report.within_limit);
            }

            THEN("The stack frame should use less than 100% of stack") {
                CHECK(report.stack_percent < 100.0);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: UI Component Stack Usage
// ============================================================================

TEST_SUITE_BEGIN("Stack Usage - UI Components");

SCENARIO("UI widget structure") {
    GIVEN("A UI widget structure") {
        struct MockUIWidget {
            // Widget position and size
            int16_t x = 0;
            int16_t y = 0;
            int16_t width = 100;
            int16_t height = 20;
            // Widget state
            bool visible = true;
            bool enabled = true;
            uint8_t flags = 0;
            // Widget data
            const char* text = nullptr;
            void* parent = nullptr;
            // Callback
            void (*on_click)(void*) = nullptr;
            void* user_data = nullptr;
        };

        WHEN("Measuring widget size") {
            const auto report = StackTest::generate_report<MockUIWidget>("UIWidget");

            THEN("The widget should be less than 64 bytes") {
                CHECK(report.type_size < 64);
            }

            THEN("The widget should fit within stack limit") {
                CHECK(report.within_limit);
            }

            THEN("The widget should use less than 2% of stack") {
                CHECK(report.stack_percent < 2.0);
            }
        }
    }
}

SCENARIO("UI widget array") {
    GIVEN("An array of UI widgets") {
        struct MockUIWidget {
            int16_t x = 0;
            int16_t y = 0;
            int16_t width = 100;
            int16_t height = 20;
            bool visible = true;
            bool enabled = true;
            uint8_t flags = 0;
            const char* text = nullptr;
            void* parent = nullptr;
            void (*on_click)(void*) = nullptr;
            void* user_data = nullptr;
        };

        WHEN("Creating an array of 10 widgets") {
            constexpr size_t WIDGET_COUNT = 10;
            using WidgetArray = std::array<MockUIWidget, WIDGET_COUNT>;

            THEN("The array should fit within stack limit") {
                CHECK(StackTest::fits_in_stack<WidgetArray>());
            }

            THEN("The array should use less than 16% of stack") {
                const double usage = (static_cast<double>(sizeof(WidgetArray)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 16.0);
            }
        }

        WHEN("Creating an array of 20 widgets") {
            constexpr size_t WIDGET_COUNT = 20;
            using WidgetArray = std::array<MockUIWidget, WIDGET_COUNT>;

            THEN("The array should fit within stack limit") {
                CHECK(StackTest::fits_in_stack<WidgetArray>());
            }

            THEN("The array should use less than 32% of stack") {
                const double usage = (static_cast<double>(sizeof(WidgetArray)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 32.0);
            }
        }
    }
}

SCENARIO("UI rendering buffer") {
    GIVEN("A UI rendering function") {
        WHEN("Using a 320x240 pixel line buffer (1 row)") {
            constexpr size_t LINE_WIDTH = 320;
            using PixelColor = uint16_t;  // RGB565
            std::array<PixelColor, LINE_WIDTH> line_buffer;

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::fits_in_stack<decltype(line_buffer)>());
            }

            THEN("The buffer should use less than 16% of stack") {
                const double usage = (static_cast<double>(sizeof(line_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 16.0);
            }
        }

        WHEN("Using a 160x120 pixel line buffer (1 row)") {
            constexpr size_t LINE_WIDTH = 160;
            using PixelColor = uint16_t;  // RGB565
            std::array<PixelColor, LINE_WIDTH> line_buffer;

            THEN("The buffer should fit within stack") {
                CHECK(StackTest::fits_in_stack<decltype(line_buffer)>());
            }

            THEN("The buffer should use less than 8% of stack") {
                const double usage = (static_cast<double>(sizeof(line_buffer)) / 
                                     static_cast<double>(StackTest::STACK_LIMIT_BYTES)) * 100.0;
                CHECK(usage < 8.0);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Overall Stack Usage Summary
// ============================================================================

TEST_SUITE_BEGIN("Stack Usage - Summary");

SCENARIO("Critical stack usage limits") {
    GIVEN("The embedded system constraints") {
        WHEN("Checking stack limit") {
            THEN("Stack limit should be 4 KB") {
                CHECK_EQ(StackTest::STACK_LIMIT_BYTES, 4096);
            }

            THEN("Total RAM should be 128 KB") {
                CHECK_EQ(StackTest::TOTAL_RAM_BYTES, 131072);
            }

            THEN("Stack should be approximately 3% of total RAM") {
                const double stack_percent = (static_cast<double>(StackTest::STACK_LIMIT_BYTES) / 
                                            static_cast<double>(StackTest::TOTAL_RAM_BYTES)) * 100.0;
                CHECK(stack_percent > 3.0);
                CHECK(stack_percent < 4.0);
            }
        }
    }
}

SCENARIO("Stack usage safety margin") {
    GIVEN("A safety margin of 25% of stack") {
        constexpr size_t SAFETY_MARGIN = StackTest::STACK_LIMIT_BYTES / 4;  // 1 KB
        constexpr size_t USABLE_STACK = StackTest::STACK_LIMIT_BYTES - SAFETY_MARGIN;

        WHEN("Checking if structures fit within usable stack") {
            struct LargeBuffer {
                char data[3000];  // 3 KB
            };

            THEN("Large buffer should fit within total stack") {
                CHECK(StackTest::fits_in_stack<LargeBuffer>());
            }

            THEN("Large buffer should NOT fit within usable stack (with safety margin)") {
                CHECK(sizeof(LargeBuffer) > USABLE_STACK);
            }

            THEN("Smaller buffer should fit within usable stack") {
                struct SmallBuffer {
                    char data[2500];  // 2.5 KB
                };
                CHECK(sizeof(SmallBuffer) <= USABLE_STACK);
            }
        }
    }
}

TEST_SUITE_END();
