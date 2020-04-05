@echo off
pushd %~dp0\build_release
ninja test %* || goto :error
popd
exit /b 0
:error
popd
exit /b 1
