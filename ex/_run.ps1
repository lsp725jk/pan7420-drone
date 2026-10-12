param([string[]]$Files)
# run via ASCII junction C:\cccwork (check.bat's if(...) blocks cannot handle paths with "(1)")
$checker = "C:\py32-tools\check.bat"
$dir     = "C:\cccwork\ex"
foreach ($f in $Files) {
    Write-Host "############ $f ############"
    $log = "$env:TEMP\run_$f.txt"
    cmd /c "$checker $dir\$f.cpp c++latest > `"$log`" 2>&1"
    $lines = @(Get-Content $log -Encoding UTF8)
    $errCount = @($lines | Where-Object { $_ -match 'error C\d+' }).Count
    if ($errCount -gt 0) {
        Write-Host "[ERRORS: $errCount] first 8:"
        $lines | Where-Object { $_ -match 'error C\d+' -or $_ -match 'warning C\d+' } | Select-Object -First 8
    } else {
        $lines | ForEach-Object { $_ }
    }
}
