/*
 * Enhanced Drone Analyzer Scanner App
 * Fixed main.cpp for Mayhem Firmware
 */

#include "ui_navigation.hpp"
#include "external_app.hpp"
#include "portapack.hpp"

// Подключаем ваш объединенный заголовочный файл, где объявлен EnhancedDroneSpectrumAnalyzerView
#include "ui_scanner_combined.hpp"

// Если VERSION_MD5 не определен системой сборки, задаем заглушку
#ifndef VERSION_MD5
#define VERSION_MD5 1
#endif

using namespace portapack;

namespace ui::external_app::enhanced_drone_analyzer {

// Функция инициализации, которая будет вызвана при запуске приложения
void initialize_app(ui::NavigationView& nav) {
    // Запускаем основное окно (View), определенное в ui_scanner_combined.hpp
    nav.push<EnhancedDroneSpectrumAnalyzerView>();
}

} // namespace ui::external_app::enhanced_drone_analyzer

// Структура регистрации внешнего приложения
extern "C" {

// Атрибут section важен для того, чтобы загрузчик нашел эту структуру в начале бинарного файла
__attribute__((section(".external_app.app_enhanced_drone_analyzer_scanner.application_information"), used)) 
application_information_t _application_information_enhanced_drone_analyzer_scanner = {
    
    /*.memory_location = */ (uint8_t*)0x00000000, // Заполняется загрузчиком
    
    // Указатель на функцию запуска
    /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer::initialize_app, 
    
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "EDA Scanner",

    // Иконка 32x32
    /*.bitmap_data = */ {
        0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 0xF0, 0x0F, 0xF8, 0x1F,
        0xFC, 0x3F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFC, 0x3F, 0xF8, 0x1F, 0xF0, 0x0F,
        0xE0, 0x07, 0xC0, 0x03, 0x80, 0x01, 0x00, 0x00
    },

    /*.icon_color = */ ui::Color::red().v,
    /*.menu_location = */ app_location_t::RX, // Раздел Receivers
    /*.desired_menu_position = */ 1,          // Позиция в меню (по возможности)

    // !!! ИСПРАВЛЕНИЕ ОШИБКИ !!!
    // Число 0x4D465750 заменено на массив символов.
    // PWFM - это тег приложения Wideband FM, который нужен для работы спектра
    /*.m4_app_tag = */ { 'P', 'W', 'F', 'M' },

    /*.m4_app_offset = */ 0x00000000 // Заполняется при компиляции
};

}
