@echo off
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
pushd %~dp0\out\%mode% 2> NUL && goto :test
call %~dp0\configure.bat %mode% || exit /b 1
pushd %~dp0\out\%mode% || exit /b 1

:test
ninja test %ninja_args% || goto :error
popd
exit /b 0

:error
popd
exit /b 1
