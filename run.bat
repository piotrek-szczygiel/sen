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
set sen_args=%*
goto :after_args

:get_args
for /f "tokens=1,* delims= " %%a in ("%*") do set sen_args=%%b

:after_args
pushd %~dp0\out\%mode% 2> NUL && goto :run
call %~dp0\configure.bat %mode% || exit /b 1
pushd %~dp0\out\%mode% || exit /b 1

:run
ninja || goto :error
popd
echo+
REM pushd %~dp0
%~dp0\out\%mode%\src\sen.exe %sen_args%
REM if %errorlevel% neq 0 echo Exit code %errorlevel%
REM popd
exit /b 0

:error
popd
exit /b 1
