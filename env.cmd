@echo off

setlocal
set PATH=%~dp0scripts;%PATH%

if "%CXX%" == "" goto :search

cmd.exe /k set prompt=(sen %CXX%) $P$G
exit /b

:search
set CXX=cl
if /i "%1" == "clang" set CXX=clang-cl

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -property installationPath`) do (
    if exist "%%i" (
        set vcvarsall=%%i\VC\Auxiliary\Build\vcvarsall.bat
    )
)

if "%vcvarsall%" == "" goto :error

cmd.exe /k set prompt=(sen %CXX%) $P$G ^&^& call "%vcvarsall%" x64
exit /b

:error
echo Unable to find Visual Studio installation
echo Set CXX environment variable to manually specify the compiler
