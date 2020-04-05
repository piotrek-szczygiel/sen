@echo off
pushd %~dp0\build
ninja || goto :error
popd
echo+
%~dp0\build\src\lang.exe %*
exit /b 0
:error
popd
exit /b 1
