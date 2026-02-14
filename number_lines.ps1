param([string]$FilePath)

$lines = [System.IO.File]::ReadAllLines($FilePath, [System.Text.Encoding]::UTF8)
$output = @()
$i = 1
foreach ($line in $lines) {
    $output += "${i}: $line"
    $i++
}
$output
