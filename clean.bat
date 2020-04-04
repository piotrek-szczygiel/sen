@echo off
rd /s /q build 2> NUL
pushd src
del *.exe *.ilk *.pdb 2> NUL
popd
