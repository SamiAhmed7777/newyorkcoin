@echo off
setlocal enabledelayedexpansion

echo ===== NewYorkCoin Windows Build Setup =====
echo.

:: Create a log file
set "LOGFILE=%~dp0setup_log.txt"
echo Setup started at %date% %time% > %LOGFILE%

:: Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: This script must be run as Administrator.
    echo Please right-click and select "Run as administrator"
    echo.
    pause
    exit /b 1
)

:: Check if MSYS2 is installed
if not exist "C:\msys64" (
    echo Step 1: Installing MSYS2...
    echo Downloading MSYS2 installer...
    
    powershell -Command "& {Add-Type -AssemblyName System.Windows.Forms; [System.Windows.Forms.MessageBox]::Show('About to download MSYS2 installer. This may take a few minutes depending on your internet connection.', 'NewYorkCoin Setup')}"
    
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe' -OutFile 'msys2-installer.exe'" >> %LOGFILE% 2>&1
    
    if not exist "msys2-installer.exe" (
        echo ERROR: Failed to download MSYS2 installer.
        echo Please check your internet connection and try again.
        pause
        exit /b 1
    )
    
    echo Installing MSYS2...
    start /wait msys2-installer.exe --confirm-command
    
    if not exist "C:\msys64" (
        echo ERROR: MSYS2 installation failed.
        echo Please check %LOGFILE% for details.
        pause
        exit /b 1
    )
    
    del msys2-installer.exe
) else (
    echo Step 1: MSYS2 is already installed
)

echo.
echo Step 2: Installing required packages...

:: Set up paths
set "PATH=C:\msys64\usr\bin;C:\msys64\mingw64\bin;%PATH%"

:: Create package installation script
echo Creating package installation script...
(
echo echo "Updating package database..."
echo pacman --noconfirm -Syu
echo echo "Installing development tools..."
echo pacman --noconfirm -S mingw-w64-x86_64-toolchain
echo pacman --noconfirm -S mingw-w64-x86_64-boost
echo pacman --noconfirm -S mingw-w64-x86_64-openssl
echo pacman --noconfirm -S mingw-w64-x86_64-libevent
echo pacman --noconfirm -S mingw-w64-x86_64-db
echo pacman --noconfirm -S mingw-w64-x86_64-zmq
echo pacman --noconfirm -S mingw-w64-x86_64-miniupnpc
echo pacman --noconfirm -S make
echo pacman --noconfirm -S autoconf
echo pacman --noconfirm -S automake
echo pacman --noconfirm -S libtool
) > msys2_packages.sh

:: Run the package installation script in MSYS2
echo Installing packages (this may take a while)...
C:\msys64\usr\bin\bash.exe --login msys2_packages.sh >> %LOGFILE% 2>&1

:: Clean up
del msys2_packages.sh

echo.
echo Setup completed! 
echo.
echo IMPORTANT: You must restart your computer before building.
echo After restarting, run windows_build.bat to compile NewYorkCoin.
echo.
echo Press any key to exit...
pause > nul 