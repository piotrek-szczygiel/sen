@echo off
pushd %~dp0
meson build %* || goto :error
popd
exit /b 0
:error
popd
exit /b 1
