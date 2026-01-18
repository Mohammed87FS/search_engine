@echo off
REM Creates a portable package with the executable and all required DLLs
echo Creating portable package...
echo.

if not exist build\notesearch_gui.exe (
    echo ERROR: notesearch_gui.exe not found in build directory!
    echo Please build the project first using build_gui.bat
    pause
    exit /b 1
)

REM Create portable directory
if exist portable_package rmdir /s /q portable_package
mkdir portable_package

echo Copying executable...
copy build\notesearch_gui.exe portable_package\ >nul

echo Copying required DLLs...
if exist build\libgcc_s_seh-1.dll (
    copy /Y build\libgcc_s_seh-1.dll portable_package\ >nul
    if exist portable_package\libgcc_s_seh-1.dll (
        echo   [OK] libgcc_s_seh-1.dll
    ) else (
        echo   [ERROR] Failed to copy libgcc_s_seh-1.dll
    )
) else (
    echo   [WARNING] libgcc_s_seh-1.dll not found!
)
if exist build\libstdc++-6.dll (
    copy /Y build\libstdc++-6.dll portable_package\ >nul
    if exist portable_package\libstdc++-6.dll (
        echo   [OK] libstdc++-6.dll
    ) else (
        echo   [ERROR] Failed to copy libstdc++-6.dll
    )
) else (
    echo   [WARNING] libstdc++-6.dll not found!
)
if exist build\libwinpthread-1.dll (
    copy /Y build\libwinpthread-1.dll portable_package\ >nul
    if exist portable_package\libwinpthread-1.dll (
        echo   [OK] libwinpthread-1.dll
    ) else (
        echo   [ERROR] Failed to copy libwinpthread-1.dll
    )
) else (
    echo   [WARNING] libwinpthread-1.dll not found!
)

echo.
echo ========================================
echo Portable package created successfully!
echo ========================================
echo.
echo Location: portable_package\
echo.
echo Contents:
dir /b portable_package\
echo.
echo To distribute: Copy the entire 'portable_package' folder to any Windows computer.
echo The executable will work as long as all files are in the same directory.
echo.
pause
