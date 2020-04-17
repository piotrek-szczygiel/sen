@echo off
set root=%~dp0..

if /i "%1" == "debug" (
    set mode=debug
) else if /i "%1" == "release" (
    set mode=release
) else if "%1" == "" (
    set mode=debug
) else (
    echo Unknown configuration type: %1
    exit /b 1
)

set out=%root%\out\%mode%

call :configure
call :build
exit /b %errorlevel%

:build
cmake --build "%out%"
exit /b %errorlevel%

:configure
if exist "%out%" exit /b

if %mode% == debug   set capitalized=Debug
if %mode% == release set capitalized=Release

mkdir "%out%"
pushd "%out%"
cmake "%root%" -G Ninja -DCMAKE_BUILD_TYPE=%capitalized%
popd
exit /b %errorlevel%
