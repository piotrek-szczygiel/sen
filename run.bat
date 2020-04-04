@echo off
call build || exit /b 1
echo+
%~dp0\build\src\xd.exe
