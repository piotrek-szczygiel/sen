@echo off

setlocal
set PATH=%~dp0scripts;%PATH%

if "%1" == "vs" goto :vs

cmd.exe /k set prompt=(sen) $P$G
exit /b

:vs
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -property installationPath`) do (
    if exist "%%i" (
        set vcvarsall=%%i\VC\Auxiliary\Build\vcvarsall.bat
    )
)

if "%vcvarsall%" == "" goto :error

set CXX=cl

cmd.exe /k set prompt=(sen msvc) $P$G ^&^& call "%vcvarsall%" x64
exit /b

:error
echo Unable to find Visual Studio installation
echo CMake will try to use other installed compilers
echo Set CC or CXX environment variable to manually specify the compiler
cmd.exe /k set prompt=(sen) $P$G
