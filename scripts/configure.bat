@echo off
set root=%~dp0..

if [%1]==[] goto usage
for /f "tokens=1,* delims= " %%a in ("%*") do set MESON_ARGS=%%b

if "%1"=="debug" (
    rd /s /q %root%\out\debug 2> NUL
    meson %root%\out\debug %MESON_ARGS%
) else if "%1"=="release" (
    rd /s /q %root%\out\release 2> NUL
    meson %root%\out\release --buildtype release %MESON_ARGS%
) else (
    goto usage
)

exit /b

:usage
echo Usage: configure (debug^|release) [meson arguments...]
exit /b 1
