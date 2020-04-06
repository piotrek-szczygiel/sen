@echo off
if [%1]==[] goto usage
for /f "tokens=1,* delims= " %%a in ("%*") do set NINJA_ARGS=%%b

pushd %~dp0\out\%1 2> NUL && goto :build
call %~dp0\configure.bat %1 || exit /b 1
pushd %~dp0\out\%1 || exit /b 1

:build
ninja %NINJA_ARGS% || goto :error
popd
exit /b 0

:error
popd
exit /b 1

:usage
echo Usage: build (debug^|release) [ninja arguments...]
exit /b 1
