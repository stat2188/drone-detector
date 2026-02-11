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

static constexpr TranslationEntry TRANSLATIONS[] = {
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

static constexpr size_t TRANSLATION_COUNT =
    sizeof(TRANSLATIONS) / sizeof(TranslationEntry);

// Быстрый поиск по ключу (O(n) где n=10, быстрее чем std::string)
static constexpr const char* lookup_translation(const char* key) {
    if (!key) return nullptr;

    for (size_t i = 0; i < TRANSLATION_COUNT; ++i) {
        if (strcmp(key, TRANSLATIONS[i].key) == 0) {
            return TRANSLATIONS[i].value;
        }
    }
    return key;
}

// Translator implementations
Language Translator::current_language_ = Language::ENGLISH;

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
    return lookup_translation(key);
}

} // namespace ui::apps::enhanced_drone_analyzer
