#include "ui_drone_common_types.hpp"
#include <string>

namespace ui::apps::enhanced_drone_analyzer {

// Translator implementations
Language Translator::current_language_ = Language::ENGLISH;

const char* Translator::get_english(const std::string& key) {
    if (key == "load_database") return "Load Database";
    if (key == "save_frequency") return "Save Frequency";
    if (key == "advanced") return "Advanced";
    if (key == "constant_settings") return "Constant Settings";
    if (key == "select_language") return "Select Language";
    if (key == "about_author") return "About Author";
    if (key == "english") return "English";
    if (key == "russian") return "Russian";
    if (key == "english_selected") return "Language updated to English";
    if (key == "russian_selected") return "Язык изменен на русский";
    return key.c_str();
}

const char* Translator::get_russian(const std::string& key) {
    if (key == "load_database") return "Load Database";
    if (key == "save_frequency") return "Save Frequency";
    if (key == "advanced") return "Advanced";
    if (key == "constant_settings") return "Constant Settings";
    if (key == "select_language") return "Select Language";
    if (key == "about_author") return "About Author";
    if (key == "english") return "English";
    if (key == "russian") return "Russian";
    if (key == "english_selected") return "Language updated to English";
    if (key == "russian_selected") return "Language updated to Russian";
    return key.c_str();
}

void Translator::set_language(Language lang) {
    current_language_ = lang;
}

Language Translator::get_language() {
    return current_language_;
}

const char* Translator::translate(const std::string& key) {
    return get_translation(key);
}

const char* Translator::get_translation(const std::string& key) {
    if (current_language_ == Language::RUSSIAN) {
        return get_russian(key);
    } else {
        return get_english(key);
    }
}

} // namespace ui::apps::enhanced_drone_analyzer
