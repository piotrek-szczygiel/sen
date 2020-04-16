@echo off
set root=%~dp0..

if "%1" == "" goto :default

if /i %1 == debug (
    set mode=debug
    goto :get_args
) else if /i %1 == release (
    set mode=release
    goto :get_args
)

:default
set mode=debug
set ninja_args=%*
goto :after_args

:get_args
for /f "tokens=1,* delims= " %%a in ("%*") do set ninja_args=%%b

:after_args
pushd %root%\out\%mode% 2> NUL && goto :build
call %root%\scripts\configure.bat %mode% || exit /b 1
pushd %root%\out\%mode% || exit /b 1

:build
ninja %ninja_args% || goto :error
popd
exit /b

:error
popd
exit /b 1
