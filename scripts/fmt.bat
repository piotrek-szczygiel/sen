@echo off
set root=%~dp0..

pushd "%root%"
python "%root%\scripts\fmt.py"
popd
