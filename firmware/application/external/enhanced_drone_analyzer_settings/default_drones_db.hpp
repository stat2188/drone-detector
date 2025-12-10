// default_drones_db.hpp

#pragma once

namespace ui::external_app::enhanced_drone_analyzer {

// Этот огромный строковый литерал будет скомпилирован в бинарник.
// Формат: FREQMAN (Frequency, Description)
constexpr const char* DEFAULT_DRONE_DATABASE_CONTENT =
    "# EDA Factory Drone Database\n"
    "# Format: Freq(Hz), Description\n"
    "\n"
    "# --- DJI OcuSync / Lightbridge (2.4GHz) ---\n"
    "2406500000,DJI CH 1\n"
    "2416500000,DJI CH 2\n"
    "2426500000,DJI CH 3\n"
    "2436500000,DJI CH 4\n"
    "2446500000,DJI CH 5\n"
    "2456500000,DJI CH 6\n"
    "2466500000,DJI CH 7\n"
    "2476500000,DJI CH 8\n"
    "\n"
    "# --- Parrot / WiFi Drones (2.4GHz) ---\n"
    "2412000000,WiFi CH 1 (Parrot)\n"
    "2437000000,WiFi CH 6 (Parrot)\n"
    "2462000000,WiFi CH 11 (Parrot)\n"
    "2472000000,WiFi CH 13 (EU)\n"
    "\n"
    "# --- FPV Analog / Digital (5.8GHz) ---\n"
    "# RaceBand (Most common)\n"
    "5658000000,FPV RB CH1\n"
    "5695000000,FPV RB CH2\n"
    "5732000000,FPV RB CH3\n"
    "5769000000,FPV RB CH4\n"
    "5806000000,FPV RB CH5\n"
    "5843000000,FPV RB CH6\n"
    "5880000000,FPV RB CH7\n"
    "5917000000,FPV RB CH8\n"
    "\n"
    "# DJI FPV System (Digital)\n"
    "5660000000,DJI FPV CH1\n"
    "5695000000,DJI FPV CH2\n"
    "5735000000,DJI FPV CH3\n"
    "5770000000,DJI FPV CH4\n"
    "5805000000,DJI FPV CH5\n"
    "5839000000,DJI FPV CH6\n"
    "5878000000,DJI FPV CH7\n"
    "5914000000,DJI FPV CH8\n"
    "\n"
    "# --- Long Range Telemetry (868/915 MHz) ---\n"
    "868000000,TBS Crossfire EU\n"
    "915000000,TBS Crossfire US\n"
    "868400000,ELRS EU Start\n"
    "915400000,ELRS US Start\n"
    "\n"
    "# --- Legacy / Other ---\n"
    "433050000,LRS 433 Control\n"
    "5200000000,DJI 5.2G Aux\n"
    "5800000000,Generic 5.8 Center\n";

} // namespace
