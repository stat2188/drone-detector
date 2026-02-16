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

static constexpr TranslationEntry ENGLISH_TRANSLATIONS[] = {
    {"load_database", "Load Database"},
    {"save_frequency", "Save Frequency"},
    {"advanced", "Advanced"},
    {"constant_settings", "Constant Settings"},
    {"select_language", "Select Language"},
    {"about_author", "About Author"},
    {"english", "English"},
    {"english_selected", "Language updated to English"}
};

static constexpr size_t ENGLISH_TRANSLATION_COUNT =
    sizeof(ENGLISH_TRANSLATIONS) / sizeof(TranslationEntry);

// Fast key-based lookup (O(n) where n=10, faster than std::string)
static constexpr const char* lookup_translation(
    const char* key,
    const TranslationEntry* table,
    size_t count
) {
    if (!key) return nullptr;
    
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(key, table[i].key) == 0) {
            return table[i].value;
        }
    }
    return key;
}

// Translator implementations
Language Translator::current_language_ = Language::ENGLISH;

const char* Translator::get_english(const char* key) {
    return lookup_translation(key, ENGLISH_TRANSLATIONS, ENGLISH_TRANSLATION_COUNT);
}

void Translator::set_language(Language lang) {
    current_language_ = lang;
}

Language Translator::get_language() {
    return current_language_;
}

const char* Translator::translate(const char* key) {
    return get_translation(key);
}

const char* Translator::get_translation(const char* key) {
    return get_english(key);
}

} // namespace ui::apps::enhanced_drone_analyzer
