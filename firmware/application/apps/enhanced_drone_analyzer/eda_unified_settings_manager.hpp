/*
 * EDA Unified Settings Manager - Handles persistence for EDAUnifiedSettings
 */

#ifndef __EDA_UNIFIED_SETTINGS_MANAGER_HPP__
#define __EDA_UNIFIED_SETTINGS_MANAGER_HPP__

#include "eda_unified_settings.hpp"
#include <string>
#include <cstddef>
#include <cstdio>

class File;

namespace ui::apps::enhanced_drone_analyzer {

class EDAUnifiedSettingsManager {
public:
    static bool load(EDAUnifiedSettings& settings);
    static bool save(const EDAUnifiedSettings& settings);
    static void reset_to_defaults(EDAUnifiedSettings& settings);
    static bool validate(const EDAUnifiedSettings& settings);

private:
    static constexpr char SETTINGS_FILENAME[] = "/sdcard/EDA_UNIFIED_SETTINGS.txt";
    static constexpr char SETTINGS_BACKUP_FILENAME[] = "/sdcard/EDA_UNIFIED_SETTINGS.bak";
    static constexpr char SETTINGS_HEADER[] = "# EDA Unified Settings v1.0\n";

    static bool parse_setting(const std::string& line, std::string& key, std::string& value);
    static void write_section_header(File& file, const char* section_name);
    static void write_setting(File& file, const char* key, const std::string& value);
    static void write_setting(File& file, const char* key, uint32_t value);
    static void write_setting(File& file, const char* key, int32_t value);
    static void write_setting(File& file, const char* key, bool value);
    static void write_setting(File& file, const char* key, uint64_t value);

    // 🔴 OPTIMIZATION: Pre-allocated buffer to avoid heap fragmentation
    static constexpr size_t WRITE_BUFFER_SIZE = 2048;
    static void write_setting_buffered(File& file, const char* key, const char* value,
                                       char* buffer, size_t buffer_size);
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __EDA_UNIFIED_SETTINGS_MANAGER_HPP__
