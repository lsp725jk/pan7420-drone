@echo off
REM ASCII-only: compile one C source to .obj and dump its symbol names.
REM   _symdump_c.bat <source.c>
setlocal enabledelayedexpansion
set VCVARS=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat
if not exist "%VCVARS%" set VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
call "%VCVARS%" 1>nul
if errorlevel 1 ( echo [ERR] vcvars failed & exit /b 2 )
set EXEDIR=%TEMP%\ccc_run_dsh1
if not exist "%EXEDIR%" mkdir "%EXEDIR%"
echo ===== SYMDUMP_C %~nx1 =====
cl /nologo /W4 /c /Fo"%EXEDIR%\symdumpc.obj" "%~f1" 2>&1
if errorlevel 1 ( echo ===== COMPILE_FAIL ===== & exit /b 1 )
dumpbin /nologo /symbols "%EXEDIR%\symdumpc.obj" | findstr /R /C:"External"
echo ===== ALL DONE =====
endlocal
