@echo off
setlocal enabledelayedexpansion

:: Configuration
set VCPKG_ROOT=F:\vcpkg
set BUILD_DIR=build
set CMAKE_GENERATOR=MinGW Makefiles

:: Enable ANSI and set colors
for /f %%A in ('echo prompt $E ^| cmd') do set "ESC=%%A"
set GREEN=%ESC%[92m
set RED=%ESC%[91m
set YELLOW=%ESC%[93m
set NC=%ESC%[0m

echo.

:: Check if vcpkg exists
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo %RED%Error: vcpkg not found at %VCPKG_ROOT%%NC%
    echo Please install vcpkg or update VCPKG_ROOT path in this script
    exit /b 1
)

:: Check if vcpkg.json exists
if not exist "vcpkg.json" (
    echo %RED%Error: vcpkg.json not found in current directory%NC%
    echo Please make sure you're running this from your project root
    exit /b 1
)

echo %YELLOW%Step 1: Installing vcpkg dependencies...%NC%
echo Running: %VCPKG_ROOT%\vcpkg install
echo.
"%VCPKG_ROOT%\vcpkg.exe" install
if errorlevel 1 (
    echo %RED%Error: vcpkg install failed%NC%
    exit /b 1
)

echo.
echo %GREEN%vcpkg dependencies installed successfully!%NC%
echo.

:: Create build directory
if not exist "%BUILD_DIR%" (
    echo %YELLOW%Creating build directory: %BUILD_DIR%%NC%
    mkdir "%BUILD_DIR%"
)

echo %YELLOW%Step 2: Running CMake configuration...%NC%
echo Running: cmake -B %BUILD_DIR% -S . -G "%CMAKE_GENERATOR%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
echo.
cmake -B "%BUILD_DIR%" -S . -G "%CMAKE_GENERATOR%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
if errorlevel 1 (
    echo %RED%Error: CMake configuration failed%NC%
    exit /b 1
)

echo.
echo %GREEN%CMake configuration completed successfully!%NC%
echo.

echo %YELLOW%Step 3: Building project...%NC%
echo Running: cmake --build %BUILD_DIR% --config Release
echo.
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 (
    echo %RED%Error: Build failed%NC%
    exit /b 1
)

echo.
echo %GREEN%=== Build completed successfully! ===%NC%
echo.
echo Executable should be in: %BUILD_DIR%\Release\
echo.

:: Ask if user wants to run the executable
set /p RUN_EXE=Do you want to run the executable? (y/n): 
if /i "%RUN_EXE%"=="y" (
    echo.
    echo %YELLOW%Looking for executable in %BUILD_DIR%\examples\basic_triangle...%NC%
    for %%f in ("%BUILD_DIR%\examples\basic_triangle\*.exe") do (
        echo Running: %%f
        "%%f"
        goto :eof
    )
    echo %RED%No executable found in %BUILD_DIR%\examples\basic_triangle%NC%
)

echo.
echo %GREEN%Script completed!%NC%