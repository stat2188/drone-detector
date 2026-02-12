#include "ui_drone_common_types.hpp"
#include <cstring>
#include <cstdio>

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// DIAMOND OPTIMIZATION: constexpr LUTs для переводов
// ===========================================
// Scott Meyers Item 15: Prefer constexpr to #define
// Все строки хранятся во Flash, RAM не тратится
// O(n) поиск по массиву быстрее, чем std::string сравнение

static constexpr TranslationEntry ENGLISH_TRANSLATIONS[] = {
    {"load_database", "Load Database"},
    {"save_frequency", "Save Frequency"},
    {"advanced", "Advanced"},
    {"constant_settings", "Constant Settings"},
    {"select_language", "Select Language"},
    {"about_author", "About Author"},
    {"english", "English"},
    {"russian", "Russian"},
    {"english_selected", "Language updated to English"},
    {"russian_selected", "Язык изменен на русский"}
};

static constexpr size_t ENGLISH_TRANSLATION_COUNT = 
    sizeof(ENGLISH_TRANSLATIONS) / sizeof(TranslationEntry);

static constexpr TranslationEntry RUSSIAN_TRANSLATIONS[] = {
    {"load_database", "Load Database"},
    {"save_frequency", "Save Frequency"},
    {"advanced", "Advanced"},
    {"constant_settings", "Constant Settings"},
    {"select_language", "Select Language"},
    {"about_author", "About Author"},
    {"english", "English"},
    {"russian", "Russian"},
    {"english_selected", "Language updated to English"},
    {"russian_selected", "Language updated to Russian"}
};

static constexpr size_t RUSSIAN_TRANSLATION_COUNT = 
    sizeof(RUSSIAN_TRANSLATIONS) / sizeof(TranslationEntry);

// Быстрый поиск по ключу (O(n) где n=10, быстрее чем std::string)
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

const char* Translator::get_russian(const char* key) {
    return lookup_translation(key, RUSSIAN_TRANSLATIONS, RUSSIAN_TRANSLATION_COUNT);
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
    if (current_language_ == Language::RUSSIAN) {
        return get_russian(key);
    } else {
        return get_english(key);
    }
}

} // namespace ui::apps::enhanced_drone_analyzer
