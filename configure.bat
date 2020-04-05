@echo off
call %~dp0\clean.bat
meson build
meson build_release --buildtype release
meson build_vs --backend vs2019
