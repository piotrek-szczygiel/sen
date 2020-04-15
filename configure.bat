@echo off
if [%1]==[] goto usage
for /f "tokens=1,* delims= " %%a in ("%*") do set MESON_ARGS=%%b

set out=%~dp0\out

if "%1"=="debug" (
    rd /s /q %out%\debug 2> NUL
    meson %out%\debug %MESON_ARGS%
) else if "%1"=="release" (
    rd /s /q %out%\release 2> NUL
    meson %out%\release --buildtype release %MESON_ARGS%
) else (
    goto usage
)

exit /b 0

:usage
echo Usage: configure (debug^|release) [meson arguments...]
exit /b 1
