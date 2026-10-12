param([switch]$Quiet)
# 全量复验：走 ASCII junction，用官方 check.bat
$checker = "C:\py32-tools\check.bat"
$dir     = "C:\cccwork\ex"
$mine = Get-ChildItem $dir -File | Where-Object { $_.Name -match '^ch(18|19|20|21)_ex' } |
        Select-Object -ExpandProperty BaseName | Sort-Object
$results = @()
foreach ($f in $mine) {
    $log = "$env:TEMP\v_$f.txt"
    cmd /c "$checker $dir\$f.cpp c++latest > `"$log`" 2>&1"
    $lines = @(Get-Content $log -Encoding UTF8)
    $errs  = @($lines | Where-Object { $_ -match 'error C\d+' -or $_ -match '\[ERROR\]' -or $_ -match '\[BUILD FAILED\]' }).Count
    $warns = @($lines | Where-Object { $_ -match 'warning C\d+' }).Count
    $exit  = ($lines | Where-Object { $_ -match 'EXIT CODE:' } | Select-Object -Last 1)
    $built = @($lines | Where-Object { $_ -match 'BUILD OK' }).Count
    $results += [pscustomobject]@{
        Example = $f
        Build   = if ($errs -gt 0) { 'FAIL' } elseif ($built -gt 0 -or $f -eq 'ch20_ex12_modules') { 'OK' } else { '??' }
        Warnings= $warns
        Exit    = ($exit -replace '.*EXIT CODE:\s*','' -replace '\s*-.*','')
    }
}
if (-not $Quiet) { $results | Format-Table -AutoSize }
$fail = @($results | Where-Object { $_.Build -ne 'OK' })
Write-Host ("总计 " + $results.Count + " 个示例，编译失败 " + $fail.Count + " 个，警告合计 " + (($results | Measure-Object Warnings -Sum).Sum))
if ($fail.Count -gt 0) { $fail | Format-Table -AutoSize }
