@echo off
REM ASCII-only runner: initialize MSVC once, then compile+run every .cpp in a list.
setlocal enabledelayedexpansion
set VCVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat
if not exist "%VCVARS%" set VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
call "%VCVARS%" 1>nul
if errorlevel 1 ( echo [ERR] vcvars failed & exit /b 2 )
set EXEDIR=%TEMP%\ccc_run_dsh1
if not exist "%EXEDIR%" mkdir "%EXEDIR%"
for %%F in (%*) do (
  echo ===== BEGIN %%~nxF =====
  del "%EXEDIR%\%%~nF.exe" 1>nul 2>&1
  cl /nologo /utf-8 /std:c++latest /EHsc /W4 /D_CRT_SECURE_NO_WARNINGS /Fe:"%EXEDIR%\%%~nF.exe" /Fo:"%EXEDIR%\\" "%%~fF" 2>&1
  if errorlevel 1 (
    echo ===== COMPILE_FAIL %%~nxF =====
  ) else (
    if exist "%EXEDIR%\%%~nF.exe" (
      "%EXEDIR%\%%~nF.exe" 2>&1
      echo ===== EXITCODE !ERRORLEVEL! %%~nxF =====
    ) else (
      echo ===== NO_EXE %%~nxF =====
    )
  )
)
echo ===== ALL DONE =====
endlocal
