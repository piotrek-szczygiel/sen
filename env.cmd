@echo off

setlocal
set PATH=%~dp0scripts;%PATH%

if "%1" == "" (
    cmd.exe /k "set prompt=(sen) $P$G"
    exit /b
)

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -property installationPath`) do (
    if exist "%%i" (
        set vcvarsall=%%i\VC\Auxiliary\Build\vcvarsall.bat
    )
)

if "%vcvarsall%" == "" (
    echo Unable to find Visual Studio installation
    exit /b 1
)

cmd.exe /k set prompt=(sen %1) $P$G ^&^& call "%vcvarsall%" %1
