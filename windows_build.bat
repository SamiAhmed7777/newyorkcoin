@echo off
setlocal enabledelayedexpansion

echo ===== NewYorkCoin Windows Build Process =====
echo.

:: Create a log file
set "LOGFILE=%~dp0build_log.txt"
echo Build started at %date% %time% > %LOGFILE%

:: Check if MSYS2 is installed
if not exist "C:\msys64" (
    echo ERROR: MSYS2 is not installed.
    echo Please run windows_setup.bat first as administrator.
    echo.
    pause
    exit /b 1
)

:: Check for required files
if not exist "configure.ac" (
    echo ERROR: configure.ac not found.
    echo Please make sure you're running this script from the NewYorkCoin root directory.
    echo.
    pause
    exit /b 1
)

echo Step 1: Setting up build environment...
:: Set up paths
set "PATH=C:\msys64\usr\bin;C:\msys64\mingw64\bin;%PATH%"

:: Get current directory
set "CURRENT_DIR=%CD%"
set "MSYS_CURRENT_DIR=%CURRENT_DIR:\=/%"

:: Create build script
echo Step 2: Creating build script...
(
echo cd "%MSYS_CURRENT_DIR%"
echo echo "Running autogen.sh..."
echo ./autogen.sh
echo echo "Running configure..."
echo ./configure --prefix=/mingw64 \
echo     --disable-shared \
echo     --enable-static \
echo     --with-boost=/mingw64 \
echo     --with-boost-libdir=/mingw64/lib \
echo     LDFLAGS="-L/mingw64/lib" \
echo     CPPFLAGS="-I/mingw64/include" \
echo     CXXFLAGS="-O2 -g" \
echo     --host=x86_64-w64-mingw32
echo echo "Cleaning previous build..."
echo make clean
echo echo "Building NewYorkCoin..."
echo make -j4
) > build_nyc.sh

:: Run the build script in MSYS2
echo Step 3: Building NewYorkCoin (this will take a while)...
echo This process may take 30 minutes or more depending on your system.
echo Progress will be logged to %LOGFILE%
echo.
C:\msys64\usr\bin\bash.exe --login build_nyc.sh >> %LOGFILE% 2>&1

:: Clean up
del build_nyc.sh

:: Check if build was successful
if exist "src\newyorkcoind.exe" (
    echo.
    echo ===== Build completed successfully! =====
    echo.
    echo Binaries can be found in the src directory:
    echo - src\newyorkcoind.exe (NewYorkCoin daemon)
    echo - src\newyorkcoin-cli.exe (Command-line interface)
    echo - src\test\test_newyorkcoin.exe (Test suite)
    echo.
    echo To run tests, use:
    echo src\test\test_newyorkcoin.exe --log_level=all --run_test=newyorkcoin_enhancements_tests
    echo.
    echo To start the daemon:
    echo 1. Create %APPDATA%\NewYorkCoin\newyorkcoin.conf
    echo 2. Run src\newyorkcoind.exe
    echo.
) else (
    echo.
    echo ===== Build failed! =====
    echo Please check %LOGFILE% for error messages.
    echo Common issues:
    echo 1. Missing dependencies
    echo 2. Compilation errors
    echo 3. Disk space issues
    echo.
)

echo Press any key to exit...
pause > nul 