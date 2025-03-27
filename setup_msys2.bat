@echo off
echo Installing MSYS2...

:: Download MSYS2
powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe' -OutFile 'msys2.exe'}"

:: Install MSYS2
start /wait msys2.exe --confirm-command

:: Check if installation succeeded
if exist "C:\msys64" (
    echo MSYS2 installed successfully!
) else (
    echo MSYS2 installation failed!
    pause
    exit /b 1
)

:: Install packages
C:\msys64\usr\bin\bash.exe -lc "pacman --noconfirm -Syu"
C:\msys64\usr\bin\bash.exe -lc "pacman --noconfirm -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-boost mingw-w64-x86_64-openssl mingw-w64-x86_64-libevent mingw-w64-x86_64-db mingw-w64-x86_64-zmq mingw-w64-x86_64-miniupnpc make autoconf automake libtool"

echo Setup complete! Please restart your computer.
pause 