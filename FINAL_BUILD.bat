@echo off
echo ========================================
echo NoteSearch - Final Build Script
echo ========================================
echo.

echo Checking compiler...
g++ --version
if errorlevel 1 (
    echo ERROR: g++ not found in PATH
    pause
    exit /b 1
)
echo.

echo Compiling all source files at once...
g++ -std=c++17 -O2 -I include ^
    src/tokenizer.cpp ^
    src/util.cpp ^
    src/document_store.cpp ^
    src/index.cpp ^
    src/file_scanner.cpp ^
    src/search.cpp ^
    src/main.cpp ^
    -lstdc++fs ^
    -o build/notesearch.exe

if errorlevel 1 (
    echo.
    echo ========================================
    echo BUILD FAILED
    echo ========================================
    echo.
    echo Trying to compile files individually to find the error...
    echo.
    
    g++ -std=c++17 -I include -c src/tokenizer.cpp -o build/tokenizer.o
    if errorlevel 1 echo [FAILED] tokenizer.cpp
    
    g++ -std=c++17 -I include -c src/util.cpp -o build/util.o
    if errorlevel 1 echo [FAILED] util.cpp
    
    g++ -std=c++17 -I include -c src/document_store.cpp -o build/document_store.o
    if errorlevel 1 echo [FAILED] document_store.cpp
    
    g++ -std=c++17 -I include -c src/index.cpp -o build/index.o
    if errorlevel 1 echo [FAILED] index.cpp
    
    g++ -std=c++17 -I include -c src/file_scanner.cpp -o build/file_scanner.o
    if errorlevel 1 echo [FAILED] file_scanner.cpp
    
    g++ -std=c++17 -I include -c src/search.cpp -o build/search.o
    if errorlevel 1 echo [FAILED] search.cpp
    
    g++ -std=c++17 -I include -c src/main.cpp -o build/main.o
    if errorlevel 1 echo [FAILED] main.cpp
    
    pause
    exit /b 1
)

if exist build\notesearch.exe (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Executable: build\notesearch.exe
    echo.
    echo To test:
    echo   build\notesearch.exe index examples
    echo   build\notesearch.exe search "RAII"
    echo.
) else (
    echo.
    echo ERROR: Executable not created
    pause
    exit /b 1
)

pause
