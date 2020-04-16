@echo off
set root=%~dp0..

if exist %root%\out\debug\ (
    set dir=%root%\out\debug
) else if exist %root%\out\release\ (
    set dir=%root%\out\release
) else (
    echo No build directory found!
    exit /b 1
)

pushd %dir%
ninja clang-format
popd
