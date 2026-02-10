# Enhanced Drone Analyzer (EDA) Code Collector - PowerShell
# Collects all EDA source files into a single text file

$OutputFile = "EDA_FULL_CODE.txt"
$EdaDir = "firmware\application\apps\enhanced_drone_analyzer"

# Clear or create output file
"" | Out-File -FilePath $OutputFile -Encoding UTF8

$Header = @"
========================================
Enhanced Drone Analyzer (EDA) - Full Code
Generated: $(Get-Date)
========================================

"@

$Header | Out-File -FilePath $OutputFile -Encoding UTF8 -Append

# Files to include (in logical order)
$Files = @(
    "drone_constants.hpp",
    "color_lookup_unified.hpp",
    "settings_bitfields.hpp",
    "default_drones_db.hpp",
    "ui_drone_common_types.hpp",
    "ui_drone_common_types.cpp",
    "ui_signal_processing.hpp",
    "ui_signal_processing.cpp",
    "ui_spectral_analyzer.hpp",
    "ui_drone_audio.hpp",
    "settings_persistence.hpp",
    "settings_persistence.cpp",
    "ui_enhanced_drone_memory_pool.hpp",
    "eda_advanced_settings.hpp",
    "eda_optimized_utils.hpp",
    "diamond_core.hpp",
    "scanning_coordinator.hpp",
    "ui_enhanced_drone_settings.hpp",
    "ui_enhanced_drone_settings.cpp",
    "enhanced_drone_analyzer_app.cpp",
    "ui_enhanced_drone_analyzer.hpp",
    "ui_enhanced_drone_analyzer.cpp"
)

$ProcessedCount = 0

foreach ($File in $Files) {
    $FilePath = Join-Path -Path $EdaDir -ChildPath $File

    if (Test-Path -Path $FilePath) {
        Write-Host "Processing: $File"

        $Separator = @"

========================================
FILE: $File
========================================

"@

        $Separator | Out-File -FilePath $OutputFile -Encoding UTF8 -Append

        Get-Content -Path $FilePath | Out-File -FilePath $OutputFile -Encoding UTF8 -Append

        "" | Out-File -FilePath $OutputFile -Encoding UTF8 -Append
        $ProcessedCount++
    } else {
        Write-Host "WARNING: File not found: $File" -ForegroundColor Yellow
    }
}

# Add footer
$Footer = @"

========================================
END OF EDA CODE
========================================

"@

$Footer | Out-File -FilePath $OutputFile -Encoding UTF8 -Append

Write-Host ""
Write-Host "Done! Output saved to: $OutputFile"
Write-Host "Total files processed: $ProcessedCount"
