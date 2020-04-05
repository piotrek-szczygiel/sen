@echo off
pushd %~dp0\build%*
ninja || goto :error
popd
exit /b 0
:error
popd
exit /b 1
