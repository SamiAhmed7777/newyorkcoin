# Building NewYorkCoin for Windows

This guide explains how to build NewYorkCoin for Windows x64 using the provided build scripts.

## Prerequisites

- Windows 10 or later (64-bit)
- At least 8GB of RAM
- At least 20GB of free disk space
- Internet connection

## Build Steps

1. **Setup Environment**
   - Run `windows_setup.bat` as administrator
   - This will:
     - Install MSYS2 if not already installed
     - Install all required dependencies
     - Configure the build environment
   - After setup completes, restart your computer

2. **Build NewYorkCoin**
   - Run `windows_build.bat`
   - This will:
     - Configure the build system
     - Compile NewYorkCoin and its components
     - Create Windows executables

3. **Output Files**
   The following files will be created in the `src` directory:
   - `newyorkcoind.exe` - The NewYorkCoin daemon
   - `newyorkcoin-cli.exe` - Command-line interface
   - `test/test_newyorkcoin.exe` - Test suite

## Running Tests

To run the enhancement tests:
```bash
src\test\test_newyorkcoin.exe --log_level=all --run_test=newyorkcoin_enhancements_tests
```

To run all tests:
```bash
src\test\test_newyorkcoin.exe --log_level=all
```

## Configuration

After building, create or modify `%APPDATA%\NewYorkCoin\newyorkcoin.conf`:
```conf
# Enhanced networking parameters
maxconnections=200
maxuploadtarget=5000
maxmempool=300

# UTXO cache optimization
utxocachesize=450

# Parallel validation
parallelvalidation=1
validationthreads=4

# Debug logging
debug=net
debug=mempool
debug=validation
debug=utxo
```

## Troubleshooting

1. If MSYS2 installation fails:
   - Download MSYS2 manually from https://www.msys2.org
   - Install it to C:\msys64
   - Run `windows_setup.bat` again

2. If build fails:
   - Check the error messages in the console
   - Make sure all dependencies were installed correctly
   - Try running `windows_build.bat` again

3. If tests fail:
   - Check the test output for specific failures
   - Verify configuration settings
   - Make sure all dependencies are properly installed

## Support

If you encounter any issues, please:
1. Check the error messages
2. Review the build logs
3. Verify your system meets the prerequisites
4. Create an issue in the GitHub repository with detailed information about the problem 