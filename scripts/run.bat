@echo off
set root=%~dp0..

if /i "%1" == "debug" (
    set mode=debug
    goto :get_args
) else if /i "%1" == "release" (
    set mode=release
    goto :get_args
) else (
    set mode=debug
    set sen_args=%*
    goto :run
)

:get_args
for /f "tokens=1,* delims= " %%a in ("%*") do set sen_args=%%b

:run
call "%root%\scripts\build.bat" %mode%
if errorlevel 1 exit /b 1

pushd "%root%"
"%out%\sen.exe" %sen_args%
popd
