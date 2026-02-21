// Diamond Code: Flash-resident drone frequency database

#pragma once

#include <cstdint>

namespace ui::apps::enhanced_drone_analyzer {

// Flash storage attribute for Cortex-M4
#ifdef __GNUC__
    #define EDA_FLASH_CONST __attribute__((section(".rodata")))
#else
    #define EDA_FLASH_CONST const
#endif

// Type Aliases (Semantic Types)

/**
 * @brief Type alias for database content (Flash-resident string literal)
 * @note Points to string literal in Flash (.rodata section), not heap
 */
using DatabaseContent = const char*;

/**
 * @brief Type alias for frequency values (Hz)
 * @note Uses uint64_t to support frequencies up to 7.2 GHz
 */
using FrequencyHz = uint64_t;

// Constants (Flash-Resident)

namespace DatabaseConstants {

/// @brief TBS Crossfire EU frequency (868 MHz)
constexpr FrequencyHz TBS_CROSSFIRE_EU = 868000000ULL;

/// @brief TBS Crossfire US frequency (915 MHz)
constexpr FrequencyHz TBS_CROSSFIRE_US = 915000000ULL;

/// @brief ELRS 868 MHz frequency
constexpr FrequencyHz ELRS_868 = 866000000ULL;

/// @brief ELRS 915 MHz frequency
constexpr FrequencyHz ELRS_915 = 915000000ULL;

/// @brief LRS 433 MHz control frequency
constexpr FrequencyHz LRS_433 = 433050000ULL;

/// @brief DJI OcuSync/Lightbridge Channel 1 (2.4 GHz)
constexpr FrequencyHz DJI_CH1 = 2406500000ULL;

/// @brief DJI OcuSync/Lightbridge Channel 3 (2.4 GHz)
constexpr FrequencyHz DJI_CH3 = 2416500000ULL;

/// @brief DJI OcuSync/Lightbridge Channel 5 (2.4 GHz)
constexpr FrequencyHz DJI_CH5 = 2426500000ULL;

/// @brief DJI OcuSync/Lightbridge Channel 7 (2.4 GHz)
constexpr FrequencyHz DJI_CH7 = 2436500000ULL;

/// @brief FPV RaceBand Channel 1 (5.8 GHz)
constexpr FrequencyHz FPV_RB_CH1 = 5658000000ULL;

/// @brief FPV RaceBand Channel 2 (5.8 GHz)
constexpr FrequencyHz FPV_RB_CH2 = 5695000000ULL;

/// @brief FPV RaceBand Channel 3 (5.8 GHz)
constexpr FrequencyHz FPV_RB_CH3 = 5732000000ULL;

/// @brief FPV RaceBand Channel 4 (5.8 GHz)
constexpr FrequencyHz FPV_RB_CH4 = 5769000000ULL;

/// @brief DJI FPV System Channel 1 (5.8 GHz)
constexpr FrequencyHz DJI_FPV_CH1 = 5735000000ULL;

/// @brief WiFi Channel 1 (Parrot drones)
constexpr FrequencyHz WIFI_CH1 = 2412000000ULL;

/// @brief WiFi Channel 6 (Parrot drones)
constexpr FrequencyHz WIFI_CH6 = 2437000000ULL;

/// @brief WiFi Channel 11 (Parrot drones)
constexpr FrequencyHz WIFI_CH11 = 2462000000ULL;

} // namespace DatabaseConstants

// Database Content (Flash-Resident)

/**
 * @brief Default drone database content (Flash-resident string literal)
 * @note Compiled into binary (.rodata section), zero RAM usage at runtime
 * @note Format: Freq(Hz), Description
 * @note Reduced from 31 to 15 entries for memory efficiency (~384 bytes savings)
 */
EDA_FLASH_CONST constexpr DatabaseContent DEFAULT_DRONE_DATABASE_CONTENT =
    "# EDA Factory Drone Database\n"
    "# Format: Freq(Hz), Description\n"
    "\n"
    "# --- Long Range Telemetry (868/915 MHz) ---\n"
    "868000000,TBS Crossfire EU\n"
    "915000000,TBS Crossfire US\n"
    "866000000,ELRS 868\n"
    "915000000,ELRS 915\n"
    "\n"
    "# --- Legacy / Other ---\n"
    "433050000,LRS 433 Control\n"
    "\n"
    "# --- DJI OcuSync / Lightbridge (2.4GHz) ---\n"
    "# Reduced from 8 to 4 channels (kept most common)\n"
    "2406500000,DJI CH 1\n"
    "2416500000,DJI CH 3\n"
    "2426500000,DJI CH 5\n"
    "2436500000,DJI CH 7\n"
    "\n"
    "# --- FPV Analog / Digital (5.8GHz) ---\n"
    "# RaceBand (Reduced from 8 to 4 channels - kept most popular)\n"
    "5658000000,FPV RB CH1\n"
    "5695000000,FPV RB CH2\n"
    "5732000000,FPV RB CH3\n"
    "5769000000,FPV RB CH4\n"
    "\n"
    "# DJI FPV System (Digital) - Reduced from 8 to 1 channel\n"
    "5735000000,DJI FPV CH1\n"
    "\n"
    "# --- Parrot / WiFi Drones (2.4GHz) ---\n"
    "2412000000,WiFi CH 1 (Parrot)\n"
    "2437000000,WiFi CH 6 (Parrot)\n"
    "2462000000,WiFi CH 11 (Parrot)\n";

} // namespace ui::apps::enhanced_drone_analyzer
