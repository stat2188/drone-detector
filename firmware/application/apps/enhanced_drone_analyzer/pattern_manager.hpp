#ifndef PATTERN_MANAGER_HPP
#define PATTERN_MANAGER_HPP

#include <cstdint>
#include <cstddef>

#include "ch.h"
#include "file.hpp"
#include "file_path.hpp"
#include "pattern_types.hpp"
#include "constants.hpp"
#include <array>

#include "locking.hpp"

namespace drone_analyzer {

/**
 * @brief File-backed pattern storage (mutex-protected).
 * @note CSV format: name,wave[16],features[8],threshold,flags,center_freq,range_width
 *                   (29 fields per line, one line per file in /EDA/PATTERNS/*.TXT).
 * @note Format is stable — older .TXT files load without migration.
 * @note Single source of truth for on-disk pattern data.
 *
 * Stack: 0 (all storage is class-static std::array).
 * Flash: 0 (header only).
 * SRAM: MAX_PATTERNS × sizeof(SignalPattern) ≈ 10 × 72 = 720 B.
 */
class PatternManager {
public:
    PatternManager() noexcept;
    ~PatternManager() noexcept;

    PatternManager(const PatternManager&) = delete;
    PatternManager& operator=(const PatternManager&) = delete;
    PatternManager(PatternManager&&) = delete;
    PatternManager& operator=(PatternManager&&) = delete;

    /** @brief Load all patterns from /EDA/PATTERNS/*.TXT into the in-memory array. */
    [[nodiscard]] ErrorCode load_patterns() noexcept;

    /** @brief Save one pattern to /EDA/PATTERNS/<name>.TXT and append to in-memory array. */
    [[nodiscard]] ErrorCode save_pattern(const SignalPattern& pattern) noexcept;

    /** @brief Delete a pattern by index (file + array compaction). */
    [[nodiscard]] ErrorCode delete_pattern(size_t index) noexcept;

    [[nodiscard]] const SignalPattern* get_pattern(size_t index) const noexcept;
    [[nodiscard]] const SignalPattern* get_patterns_array() const noexcept;
    [[nodiscard]] size_t get_pattern_count() const noexcept;

    /** @brief Clear in-memory array (does not delete files). */
    void clear_all_patterns() noexcept;

    /** @brief Re-scan SD card and rebuild in-memory array from scratch. */
    [[nodiscard]] ErrorCode reload_patterns() noexcept;

private:
    std::array<SignalPattern, MAX_PATTERNS> patterns_;
    size_t pattern_count_;
    mutable Mutex mutex_;

    [[nodiscard]] ErrorCode load_pattern_from_line(
        const std::filesystem::path& filepath
    ) noexcept;

    [[nodiscard]] ErrorCode parse_pattern_csv(
        const char* csv_line,
        size_t csv_length
    ) noexcept;

    [[nodiscard]] static uint8_t parse_uint8(
        const char* str,
        size_t len
    ) noexcept;

    [[nodiscard]] static uint16_t parse_uint16(
        const char* str,
        size_t len
    ) noexcept;

    [[nodiscard]] static uint32_t parse_uint32(
        const char* str,
        size_t len
    ) noexcept;

    [[nodiscard]] static uint64_t parse_uint64(
        const char* str,
        size_t len
    ) noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MANAGER_HPP
