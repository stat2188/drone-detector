param([string]$FilePath)

$i = 1
Get-Content -Encoding UTF8 $FilePath | ForEach-Object { Write-Output "${i}: $_"; $i++ }
