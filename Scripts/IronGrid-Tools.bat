@echo off
setlocal EnableExtensions
cd /d "%~dp0"

title IRON GRID - Developer Tools

:menu
cls
echo ============================================================
echo               IRON GRID - DEVELOPER TOOLS
echo ============================================================
echo.
echo   [1] Clean Project
echo       Removes generated Intermediate, Binaries, .vs and .sln
echo.
echo   [2] Low-Memory Build
echo       Builds BDFR_IronGridEditor with reduced compiler memory
echo.
echo   [3] Diagnose Build Memory
echo       Shows RAM, virtual memory, page file and disk space
echo.
echo   [0] Exit
echo.
echo ============================================================

choice /C 1230 /N /M "Select an option [1/2/3/0]: "

if errorlevel 4 goto :exit
if errorlevel 3 goto :diagnose
if errorlevel 2 goto :build
if errorlevel 1 goto :clean

goto :menu

:clean
cls
echo ============================================================
echo [1] CLEAN PROJECT
echo ============================================================
echo.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Clean-Project.ps1"
set "RESULT=%ERRORLEVEL%"
echo.
echo ------------------------------------------------------------
echo Clean Project finished with exit code %RESULT%.
echo ------------------------------------------------------------
pause
goto :menu

:build
cls
echo ============================================================
echo [2] LOW-MEMORY BUILD
echo ============================================================
echo.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Build-LowMemory.ps1"
set "RESULT=%ERRORLEVEL%"
echo.
echo ------------------------------------------------------------
echo Low-Memory Build finished with exit code %RESULT%.
echo ------------------------------------------------------------
pause
goto :menu

:diagnose
cls
echo ============================================================
echo [3] BUILD MEMORY DIAGNOSTICS
echo ============================================================
echo.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Diagnose-BuildMemory.ps1"
set "RESULT=%ERRORLEVEL%"
echo.
echo ------------------------------------------------------------
echo Diagnostics finished with exit code %RESULT%.
echo ------------------------------------------------------------
pause
goto :menu

:exit
cls
echo Exiting IRON GRID Developer Tools...
timeout /t 1 /nobreak >nul
endlocal
exit /b 0
