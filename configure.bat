@echo off
if [%1]==[] goto usage
for /f "tokens=1,* delims= " %%a in ("%*") do set MESON_ARGS=%%b

if "%1"=="debug" (
    rd /s /q %~dp0\out\debug 2> NUL
    meson %~dp0\out\debug %MESON_ARGS%
) else if "%1"=="release" (
    rd /s /q %~dp0\out\release 2> NUL
    meson %~dp0\out\release --buildtype release %MESON_ARGS%
) else (
    goto usage
)

exit /b 0

:usage
echo Usage: configure (debug^|release) [meson arguments...]
exit /b 1
