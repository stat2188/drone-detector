#include "ui_drone_common_types.hpp"
#include <cstring>
#include <cstdio>

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// DIAMOND OPTIMIZATION: constexpr LUTs for translations
// ===========================================
// Scott Meyers Item 15: Prefer constexpr to #define
// All strings stored in Flash, no RAM used
// O(n) array search faster than std::string comparison
// CRITICAL FIX: Consolidated from duplicate translation systems
// - Removed: DroneAnalyzerSettingsManager_Translations
// - Consolidated: All translations in ENGLISH_TRANSLATIONS

static constexpr TranslationEntry ENGLISH_TRANSLATIONS[] = {
    {"load_database", "Load Database"},
    {"save_frequency", "Save Frequency"},
    {"advanced", "Advanced"},
    {"constant_settings", "Constant Settings"},
    {"select_language", "Select Language"},
    {"about_author", "About Author"},
    {"english", "English"},
    {"english_selected", "Language updated to English"},
    // Consolidated from DroneAnalyzerSettingsManager_Translations
    {"save_settings", "Save Settings"},
    {"load_settings", "Load Settings"},
    {"audio_settings", "Audio Settings"},
    {"hardware_settings", "Hardware Settings"},
    {"scan_interval", "Scan Interval"},
    {"rssi_threshold", "RSSI Threshold"},
    {"spectrum_mode", "Spectrum Mode"}
};

static constexpr size_t ENGLISH_TRANSLATION_COUNT =
    sizeof(ENGLISH_TRANSLATIONS) / sizeof(TranslationEntry);

// ===========================================
// CONSTANTS
// ===========================================
namespace TranslationConstants {
    constexpr size_t NULL_KEY_RESULT = 0;
    constexpr char NULL_TERMINATOR = '\0';
    
    // CRITICAL FIX: Safe return values (Flash-resident string literals)
    // Prevents use-after-free when returning original key pointer
    constexpr const char* EMPTY_STRING = "";
    constexpr const char* NOT_FOUND_KEY = "";
}

// ===========================================
// COMPILE-TIME VALIDATION
// ===========================================
namespace TranslationValidation {

/**
 * @brief Validate translation table at compile time
 * @tparam N Table size
 * @param table Translation table
 * @return true if all entries are valid
 * @note Compile-time only, no runtime overhead
 */
template<size_t N>
static constexpr bool validate_table(
    const TranslationEntry (&table)[N]
) noexcept {
    for (size_t i = 0; i < N; ++i) {
        // CRITICAL FIX: Check for null pointers
        if (table[i].key == nullptr || table[i].value == nullptr) {
            return false;
        }
        // Check for empty strings
        if (table[i].key[0] == '\0') {
            return false;
        }
    }
    return true;
}

/**
 * @brief Compile-time string comparison for constexpr contexts
 * @param str1 First string
 * @param str2 Second string
 * @return true if strings are equal
 * @note Works in constexpr contexts (C++14 compatible)
 *       Pure character-by-character comparison, no pointer comparison
 *       All operations are constant expressions for static_assert compatibility
 */
static constexpr bool strings_equal(const char* str1, const char* str2) noexcept {
    // Character-by-character comparison (all operations are constexpr)
    while (*str1 && *str2 && *str1 == *str2) {
        ++str1;
        ++str2;
    }
    // Strings are equal if both reached null terminator
    return *str1 == '\0' && *str2 == '\0';
}

/**
 * @brief Check for duplicate keys at compile time
 * @tparam N Table size
 * @param table Translation table
 * @return true if duplicate keys found
 * @note Compile-time only, no runtime overhead
 *       Uses constexpr string comparison for static_assert compatibility
 */
template<size_t N>
static constexpr bool has_duplicate_keys(
    const TranslationEntry (&table)[N]
) noexcept {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            // CRITICAL FIX: Use constexpr string comparison
            // Pointer comparison is not a constant expression in C++14
            if (strings_equal(table[i].key, table[j].key)) {
                return true;
            }
        }
    }
    return false;
}

// CRITICAL FIX: Compile-time assertions catch table errors at build time
static_assert(
    validate_table(ENGLISH_TRANSLATIONS),
    "Translation table contains null or empty entries"
);

static_assert(
    !has_duplicate_keys(ENGLISH_TRANSLATIONS),
    "Translation table contains duplicate keys"
);

} // namespace TranslationValidation

// ===========================================
// HELPER FUNCTIONS
// ===========================================
/**
 * @brief Fast key-based lookup (O(n) where n=15, faster than std::string)
 * @tparam N Table size (compile-time constant)
 * @param key Translation key to look up
 * @param table Translation table
 * @return Translation value or NOT_FOUND_KEY if not found
 * @note Returns EMPTY_STRING for null key
 * @note Thread-safe: no shared state modified
 * @note ISR-safe: marked noexcept
 * @note Compile-time bounds checking via template parameter
 */
template<size_t N>
static constexpr const char* lookup_translation(
    const char* const key,
    const TranslationEntry (&table)[N]
) noexcept {
    // CRITICAL FIX: Guard clause - null key returns empty string
    if (!key) {
        return TranslationConstants::EMPTY_STRING;
    }
    
    // Linear search through table
    // O(n) where n=15, faster than std::map
    // CRITICAL FIX: Table entries validated at compile time
    // No need to check table[i].key for null at runtime
    for (size_t i = 0; i < N; ++i) {
        if (strcmp(key, table[i].key) == 0) {
            return table[i].value;
        }
    }
    
    // CRITICAL FIX: Not found - return safe fallback
    // Original code returned 'key' which could be freed pointer
    // Now returns Flash-resident string literal
    return TranslationConstants::NOT_FOUND_KEY;
}

// ===========================================
// TRANSLATOR IMPLEMENTATIONS
// ===========================================

/**
 * @brief Get English translation
 * @param key Translation key
 * @return Translation value or NOT_FOUND_KEY
 * @note Thread-safe: only reads static data
 * @note ISR-safe: marked noexcept
 * @note CRITICAL FIX: Added input validation
 */
const char* Translator::get_english(const char* const key) noexcept {
    // CRITICAL FIX: Guard clause - validate input
    if (!key) {
        return TranslationConstants::EMPTY_STRING;
    }
    
    return lookup_translation(key, ENGLISH_TRANSLATIONS);
}

/**
 * @brief Translate key to current language
 * @param key Translation key
 * @return Translation value or NOT_FOUND_KEY
 * @note Currently only English supported
 * @note Thread-safe: only reads static data
 * @note ISR-safe: marked noexcept
 */
const char* Translator::translate(const char* const key) noexcept {
    return get_translation(key);
}

/**
 * @brief Get translation for key in current language
 * @param key Translation key
 * @return Translation value or NOT_FOUND_KEY
 * @note Language is hardcoded to English only
 * @note Thread-safe: only reads static data
 * @note ISR-safe: marked noexcept
 * @note CRITICAL FIX: Added input validation
 */
const char* Translator::get_translation(const char* const key) noexcept {
    // CRITICAL FIX: Guard clause - validate input
    if (!key) {
        return TranslationConstants::EMPTY_STRING;
    }
    
    // Language is hardcoded to English only
    return get_english(key);
}

} // namespace ui::apps::enhanced_drone_analyzer
