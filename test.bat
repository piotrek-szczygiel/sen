@echo off
pushd %~dp0\build
ninja test %* || goto :error
popd
exit /b 0
:error
popd
exit /b 1
