#ifndef PATTERN_MANAGER_HPP
#define PATTERN_MANAGER_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>

#include "ch.h"
#include "ff.h"
#include "file.hpp"
#include "file_path.hpp"
#include "pattern_types.hpp"
#include "constants.hpp"
#include <array>

#include "locking.hpp"

namespace drone_analyzer {

class PatternManager {
public:
    PatternManager() noexcept;
    ~PatternManager() noexcept;

    PatternManager(const PatternManager&) = delete;
    PatternManager& operator=(const PatternManager&) = delete;
    PatternManager(PatternManager&&) = delete;
    PatternManager& operator=(PatternManager&&) = delete;

    [[nodiscard]] ErrorCode load_patterns() noexcept;
    [[nodiscard]] ErrorCode save_pattern(const SignalPattern& pattern) noexcept;
    [[nodiscard]] ErrorCode delete_pattern(size_t index) noexcept;

    [[nodiscard]] const SignalPattern* get_pattern(size_t index) const noexcept;
    [[nodiscard]] const SignalPattern* get_patterns_array() const noexcept;
    [[nodiscard]] size_t get_pattern_count() const noexcept;

    [[nodiscard]] size_t find_pattern_by_name(const char* name) const noexcept;

    void clear_all_patterns() noexcept;

    [[nodiscard]] bool is_loaded() const noexcept;

private:
    std::array<SignalPattern, MAX_PATTERNS> patterns_;
    size_t pattern_count_;
    AtomicFlag loaded_;
    Mutex mutex_;
    bool dir_open_{false};

    [[nodiscard]] ErrorCode load_pattern_from_line(
        const char* filepath,
        size_t filepath_length
    ) noexcept;

    [[nodiscard]] ErrorCode parse_pattern_csv(
        const char* csv_line,
        size_t csv_length
    ) noexcept;

    [[nodiscard]] bool safe_str_copy(
        char* dest,
        size_t dest_size,
        const char* src
    ) const noexcept;

    [[nodiscard]] static uint8_t parse_uint8(
        const char* str,
        size_t len
    ) noexcept;

    [[nodiscard]] static bool str_equals_ignore_case(
        const char* a,
        const char* b
    ) noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MANAGER_HPP
