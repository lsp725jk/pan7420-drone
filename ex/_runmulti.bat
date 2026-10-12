@echo off
REM ASCII-only multi-file runner.
REM   _runmulti.bat <main.cpp> <other1.cpp> [other2.cpp ...]
REM Initializes MSVC once, compiles all listed sources together, runs the exe.
setlocal enabledelayedexpansion
set VCVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat
if not exist "%VCVARS%" set VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
call "%VCVARS%" 1>nul
if errorlevel 1 ( echo [ERR] vcvars failed & exit /b 2 )
set EXEDIR=%TEMP%\ccc_run_dsh1
if not exist "%EXEDIR%" mkdir "%EXEDIR%"
set SRC=
for %%F in (%*) do set SRC=!SRC! "%%~fF"
echo ===== BEGIN %~nx1 =====
del "%EXEDIR%\_multi.exe" 1>nul 2>&1
cl /nologo /utf-8 /std:c++latest /EHsc /W4 /D_CRT_SECURE_NO_WARNINGS /Fe:"%EXEDIR%\_multi.exe" /Fo:"%EXEDIR%\\" !SRC! 2>&1
if errorlevel 1 (
  echo ===== COMPILE_FAIL %~nx1 =====
) else (
  if exist "%EXEDIR%\_multi.exe" (
    "%EXEDIR%\_multi.exe" 2>&1
    echo ===== EXITCODE !ERRORLEVEL! %~nx1 =====
  ) else (
    echo ===== NO_EXE %~nx1 =====
  )
)
echo ===== ALL DONE =====
endlocal
