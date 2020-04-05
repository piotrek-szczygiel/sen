@echo off
rd /s /q %~dp0\build 2> NUL
rd /s /q %~dp0\build_release 2> NUL
rd /s /q %~dp0\build_vs 2> NUL
