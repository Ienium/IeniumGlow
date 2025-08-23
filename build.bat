@echo off
setlocal enabledelayedexpansion

:: Configuration
set VCPKG_ROOT=F:\vcpkg
set BUILD_DIR=build
set CMAKE_GENERATOR=MinGW Makefiles

:: Default flag values
set BUILD_TYPE=Debug
set BUILD_EXAMPLES=true
set RUN_TEST=false
set CLEAN=false
set RUN_AFTER=false

:: Parse commandline arguments
:parse_args
if "%~1"=="" goto :done_parsing

if /i "%~1"=="--release" (
    set BUILD_TYPE=Release
    shift
    goto :parse_args
)

if /i "%~1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto :parse_args
)

if /i "%~1"=="--no_examples" (
    set BUILD_EXAMPLES=false
    shift
    goto :parse_args
)

if /i "%~1"=="--run_test" (
    set RUN_TEST=true
    shift
    goto :parse_args
)

if /i "%~1"=="--clean" (
    set CLEAN=true
    shift
    goto :parse_args
)

if /i "%~1"=="--run" (
    set RUN_AFTER=true
    shift
    goto :parse_args
)

:done_parsing

:: Display parsed arguments
echo Build Type: %BUILD_TYPE%
echo Build Examples: %BUILD_EXAMPLES%
echo Run unit tests: %RUN_TEST%
echo Clean Build: %CLEAN%
echo Run After Build: %RUN_AFTER%
echo.

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
:: Delete Build dir if --clean
if %CLEAN%==true (
    rmdir /s /q "./%BUILD_DIR%\"
)

:: Create build directory
if not exist "%BUILD_DIR%" (
    echo %YELLOW%Creating build directory: %BUILD_DIR%%NC%
    mkdir "%BUILD_DIR%"
)

:: Define CMake flags
set examples_flag=-DIENIUM_GLOW_BUILD_EXAMPLES=ON
if %BUILD_EXAMPLES%==false (
    set examples_flag=-DIENIUM_GLOW_BUILD_EXAMPLES=OFF
)

set tests_flag=-DIENIUM_GLOW_BUILD_TESTS=OFF -DBUILD_TESTING=OFF
if %RUN_TEST%==true (
    set tests_flag=-DIENIUM_GLOW_BUILD_TESTS=ON -DBUILD_TESTING=ON
)



echo %YELLOW%Step 2: Running CMake configuration...%NC%
echo Running: cmake %examples_flag% %tests_flag% -B %BUILD_DIR% -S . -G "%CMAKE_GENERATOR%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
echo.
cmake %examples_flag% %tests_flag%  -B "%BUILD_DIR%" -S . -G "%CMAKE_GENERATOR%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" 
if errorlevel 1 (
    echo %RED%Error: CMake configuration failed%NC%
    exit /b 1
)

echo.
echo %GREEN%CMake configuration completed successfully!%NC%
echo.

echo %YELLOW%Step 3: Building project...%NC%
echo Running: cmake --build %BUILD_DIR% --config Debug
echo.
cmake --build "%BUILD_DIR%" --config Debug
if errorlevel 1 (
    echo %RED%Error: Build failed%NC%
    exit /b 1
)

echo Running unit test
if %RUN_TEST%==true (
    cd %BUILD_DIR%
    ctest
    cd ..
)

echo.
echo %GREEN%=== Build completed successfully! ===%NC%
echo.
echo Executable should be in: %BUILD_DIR%\Release\
echo.

:: Run if --run flag
if %RUN_AFTER%==true (
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