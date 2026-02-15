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

# Get all .hpp and .cpp files automatically
$HppFiles = Get-ChildItem -Path $EdaDir -Filter "*.hpp" | Sort-Object Name
$CppFiles = Get-ChildItem -Path $EdaDir -Filter "*.cpp" | Sort-Object Name
$Files = $HppFiles + $CppFiles

$ProcessedCount = 0

foreach ($File in $Files) {
    $FilePath = $File.FullName

    Write-Host "Processing: $($File.Name)"

    $Separator = @"

========================================
FILE: $($File.Name)
========================================

"@

    $Separator | Out-File -FilePath $OutputFile -Encoding UTF8 -Append

    $lines = [System.IO.File]::ReadAllLines($FilePath, [System.Text.Encoding]::UTF8)
    $lineNumber = 1
    foreach ($line in $lines) {
        "${lineNumber}: $line" | Out-File -FilePath $OutputFile -Encoding UTF8 -Append
        $lineNumber++
    }

    "" | Out-File -FilePath $OutputFile -Encoding UTF8 -Append
    $ProcessedCount++
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
