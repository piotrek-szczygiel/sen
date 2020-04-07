@echo off

set opts=-FC -GR- -EHa- -nologo -Zi

set code=%~dp0
pushd %code%\out
cl %opts% %code%\src\main.cpp -Fesen
popd
