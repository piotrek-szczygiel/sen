@echo off
pushd %~dp0\build_release
ninja || goto :error
popd
echo+
%~dp0\build_release\src\sen.exe %*
exit /b 0
:error
popd
exit /b 1
