// default_drones_db.hpp

#pragma once

namespace ui::apps::enhanced_drone_analyzer {

// This large string literal will be compiled into the binary.
// Format: FREQMAN (Frequency, Description)
// Phase 3 Optimization: Reduced from 31 to 15 entries (~384 bytes savings)
constexpr const char* DEFAULT_DRONE_DATABASE_CONTENT =
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
