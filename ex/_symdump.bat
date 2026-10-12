@echo off
REM ASCII-only: compile one source to .obj and dump its symbol names.
REM   _symdump.bat <source> [c|/Tc]
setlocal enabledelayedexpansion
set VCVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat
if not exist "%VCVARS%" set VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
call "%VCVARS%" 1>nul
if errorlevel 1 ( echo [ERR] vcvars failed & exit /b 2 )
set EXEDIR=%TEMP%\ccc_run_dsh1
if not exist "%EXEDIR%" mkdir "%EXEDIR%"
set LANGOPT=
if /I "%~2"=="c" set LANGOPT=/Tc
echo ===== SYMDUMP %~nx1 =====
cl /nologo /utf-8 /std:c++latest /EHsc /W4 /D_CRT_SECURE_NO_WARNINGS %LANGOPT% /c /Fo:"%EXEDIR%\symdump.obj" "%~f1" 2>&1
if errorlevel 1 ( echo ===== COMPILE_FAIL ===== & exit /b 1 )
dumpbin /nologo /symbols "%EXEDIR%\symdump.obj" | findstr /R /C:"External" /C:"UNDEF" /C:"SECT"
echo ===== ALL DONE =====
endlocal
