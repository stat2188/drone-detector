// default_drones_db.hpp

#pragma once

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// TYPE ALIASES (Semantic Types)
// ========================================
using DatabaseContent = const char*;
using FrequencyHz = uint64_t;

// ========================================
// CONSTANTS
// ========================================
namespace DatabaseConstants {
    constexpr FrequencyHz TBS_CROSSFIRE_EU = 868000000ULL;
    constexpr FrequencyHz TBS_CROSSFIRE_US = 915000000ULL;
    constexpr FrequencyHz ELRS_868 = 866000000ULL;
    constexpr FrequencyHz ELRS_915 = 915000000ULL;
    constexpr FrequencyHz LRS_433 = 433050000ULL;
    constexpr FrequencyHz DJI_CH1 = 2406500000ULL;
    constexpr FrequencyHz DJI_CH3 = 2416500000ULL;
    constexpr FrequencyHz DJI_CH5 = 2426500000ULL;
    constexpr FrequencyHz DJI_CH7 = 2436500000ULL;
    constexpr FrequencyHz FPV_RB_CH1 = 5658000000ULL;
    constexpr FrequencyHz FPV_RB_CH2 = 5695000000ULL;
    constexpr FrequencyHz FPV_RB_CH3 = 5732000000ULL;
    constexpr FrequencyHz FPV_RB_CH4 = 5769000000ULL;
    constexpr FrequencyHz DJI_FPV_CH1 = 5735000000ULL;
    constexpr FrequencyHz WIFI_CH1 = 2412000000ULL;
    constexpr FrequencyHz WIFI_CH6 = 2437000000ULL;
    constexpr FrequencyHz WIFI_CH11 = 2462000000ULL;
}

// This large string literal will be compiled into binary.
// Format: FREQMAN (Frequency, Description)
// Phase 3 Optimization: Reduced from 31 to 15 entries (~384 bytes savings)
constexpr DatabaseContent DEFAULT_DRONE_DATABASE_CONTENT =
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

} // namespace
