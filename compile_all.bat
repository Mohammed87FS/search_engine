@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Compiling NoteSearch
echo ========================================
echo.

set FILES=tokenizer util document_store index file_scanner search main
set OBJS=

for %%f in (%FILES%) do (
    echo Compiling src/%%f.cpp...
    g++ -std=c++17 -I include -c src/%%f.cpp -o build/%%f.o
    if errorlevel 1 (
        echo.
        echo ERROR: Failed to compile src/%%f.cpp
        echo.
        echo Trying to get error details...
        g++ -std=c++17 -I include -c src/%%f.cpp -o build/%%f.o -v 2>&1 | findstr /C:"error" /C:"Error" /C:"ERROR"
        pause
        exit /b 1
    )
    set OBJS=!OBJS! build/%%f.o
    echo [OK]
)

echo.
echo Linking...
g++ %OBJS% -lstdc++fs -o build/notesearch.exe
if errorlevel 1 (
    echo.
    echo ERROR: Linking failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo SUCCESS! Executable: build\notesearch.exe
echo ========================================
echo.

pause
