@echo off
set CXX=clang++
pushd %~dp0
cd build 2> NUL && goto :build
meson build %* || goto :error
cd build
:build
ninja || goto :error
popd
exit /b 0
:error
popd
exit /b 1
