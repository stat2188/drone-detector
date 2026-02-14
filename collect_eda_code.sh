#!/bin/bash

# ==========================================
# Enhanced Drone Analyzer (EDA) Code Collector
# ==========================================
# This script combines all EDA source files into a single text file
# Output: EDA_FULL_CODE.txt

OUTPUT_FILE="EDA_FULL_CODE.txt"
EDA_DIR="C:/Users/Max/Desktop/M/mayhem-firmware/firmware/application/apps/enhanced_drone_analyzer"

# Clear or create output file
echo "" > "$OUTPUT_FILE"

# Files to include (in logical order)
FILES=(
    "drone_constants.hpp"
    "color_lookup_unified.hpp"
    "default_drones_db.hpp"
    "ui_drone_common_types.hpp"
    "ui_drone_common_types.cpp"
    "ui_signal_processing.hpp"
    "ui_signal_processing.cpp"
    "ui_spectral_analyzer.hpp"
    "ui_drone_audio.hpp"
    "settings_persistence.hpp"
    "settings_persistence.cpp"
    "ui_enhanced_drone_memory_pool.hpp"
    "eda_advanced_settings.hpp"
    "eda_optimized_utils.hpp"
    "diamond_core.hpp"
    "scanning_coordinator.hpp"
    "ui_enhanced_drone_settings.hpp"
    "ui_enhanced_drone_settings.cpp"
    "enhanced_drone_analyzer_app.cpp"
    "ui_enhanced_drone_analyzer.hpp"
    "ui_enhanced_drone_analyzer.cpp"
)

echo "========================================" | tee -a "$OUTPUT_FILE"
echo "Enhanced Drone Analyzer (EDA) - Full Code" | tee -a "$OUTPUT_FILE"
echo "Generated: $(date)" | tee -a "$OUTPUT_FILE"
echo "========================================" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

# Process each file
for file in "${FILES[@]}"; do
    filepath="$EDA_DIR/$file"

    if [ -f "$filepath" ]; then
        echo "Processing: $file"

        echo "" | tee -a "$OUTPUT_FILE"
        echo "========================================" | tee -a "$OUTPUT_FILE"
        echo "FILE: $file" | tee -a "$OUTPUT_FILE"
        echo "========================================" | tee -a "$OUTPUT_FILE"
        echo "" | tee -a "$OUTPUT_FILE"

        cat "$filepath" >> "$OUTPUT_FILE"
        echo "" | tee -a "$OUTPUT_FILE"
    else
        echo "WARNING: File not found: $file"
    fi
done

echo "" | tee -a "$OUTPUT_FILE"
echo "========================================" | tee -a "$OUTPUT_FILE"
echo "END OF EDA CODE" | tee -a "$OUTPUT_FILE"
echo "========================================" | tee -a "$OUTPUT_FILE"

echo ""
echo "Done! Output saved to: $OUTPUT_FILE"
echo "Total files processed: ${#FILES[@]}"
