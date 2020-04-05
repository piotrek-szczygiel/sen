@echo off
rd /s /q %~dp0\build 2> NUL
pushd %~dp0\src
del *.exe *.ilk *.pdb 2> NUL
popd
