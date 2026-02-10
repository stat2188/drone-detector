@echo off
REM =========================================
REM Enhanced Drone Analyzer (EDA) Code Collector - Windows Batch
REM =========================================

setlocal EnableDelayedExpansion

set "OUTPUT_FILE=EDA_FULL_CODE.txt"
set "EDA_DIR=firmware\application\apps\enhanced_drone_analyzer"

echo. > "%OUTPUT_FILE%"

echo ======================================== >> "%OUTPUT_FILE%"
echo Enhanced Drone Analyzer (EDA) - Full Code >> "%OUTPUT_FILE%"
echo Generated: %date% %time% >> "%OUTPUT_FILE%"
echo ======================================== >> "%OUTPUT_FILE%"
echo. >> "%OUTPUT_FILE%"

REM Files to include (in logical order)
set FILES[0]=drone_constants.hpp
set FILES[1]=color_lookup_unified.hpp
set FILES[2]=settings_bitfields.hpp
set FILES[3]=default_drones_db.hpp
set FILES[4]=ui_drone_common_types.hpp
set FILES[5]=ui_drone_common_types.cpp
set FILES[6]=ui_signal_processing.hpp
set FILES[7]=ui_signal_processing.cpp
set FILES[8]=ui_spectral_analyzer.hpp
set FILES[9]=ui_drone_audio.hpp
set FILES[10]=settings_persistence.hpp
set FILES[11]=settings_persistence.cpp
set FILES[12]=ui_enhanced_drone_memory_pool.hpp
set FILES[13]=eda_advanced_settings.hpp
set FILES[14]=eda_optimized_utils.hpp
set FILES[15]=diamond_core.hpp
set FILES[16]=scanning_coordinator.hpp
set FILES[17]=ui_enhanced_drone_settings.hpp
set FILES[18]=ui_enhanced_drone_settings.cpp
set FILES[19]=enhanced_drone_analyzer_app.cpp
set FILES[20]=ui_enhanced_drone_analyzer.hpp
set FILES[21]=ui_enhanced_drone_analyzer.cpp

for /L %%i in (0,1,21) do (
    set "currentfile=!FILES[%%i]!"
    set "filepath=%EDA_DIR%\!currentfile!"

    if exist "!filepath!" (
        echo Processing: !currentfile!

        echo. >> "%OUTPUT_FILE%"
        echo ======================================== >> "%OUTPUT_FILE%"
        echo FILE: !currentfile! >> "%OUTPUT_FILE%"
        echo ======================================== >> "%OUTPUT_FILE%"
        echo. >> "%OUTPUT_FILE%"

        type "!filepath!" >> "%OUTPUT_FILE%"
        echo. >> "%OUTPUT_FILE%"
    ) else (
        echo WARNING: File not found: !currentfile!
    )
)

echo. >> "%OUTPUT_FILE%"
echo ======================================== >> "%OUTPUT_FILE%"
echo END OF EDA CODE >> "%OUTPUT_FILE%"
echo ======================================== >> "%OUTPUT_FILE%"

echo.
echo Done! Output saved to: %OUTPUT_FILE%
echo Total files processed: 22
pause
