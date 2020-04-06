@echo off
if [%1]==[] goto usage
for /f "tokens=1,* delims= " %%a in ("%*") do set SEN_ARGS=%%b

pushd %~dp0\out\%1 2> NUL && goto :run
call %~dp0\configure.bat %1 || exit /b 1
pushd %~dp0\out\%1 || exit /b 1

:run
ninja || goto :error
popd
echo+
%~dp0\out\%1\src\sen.exe %SEN_ARGS%
exit /b 0

:error
popd
exit /b 1

:usage
echo Usage: run (debug^|release) [sen arguments...]
exit /b 1
