@echo off
echo Compiling NoteSearch...
echo.

g++ -std=c++17 -I include -c src/tokenizer.cpp -o build/tokenizer.o 2>&1
if errorlevel 1 (
    echo ERROR compiling tokenizer.cpp
    pause
    exit /b 1
)
echo [OK] tokenizer.cpp

g++ -std=c++17 -I include -c src/util.cpp -o build/util.o 2>&1
if errorlevel 1 (
    echo ERROR compiling util.cpp
    pause
    exit /b 1
)
echo [OK] util.cpp

g++ -std=c++17 -I include -c src/document_store.cpp -o build/document_store.o 2>&1
if errorlevel 1 (
    echo ERROR compiling document_store.cpp
    pause
    exit /b 1
)
echo [OK] document_store.cpp

g++ -std=c++17 -I include -c src/index.cpp -o build/index.o 2>&1
if errorlevel 1 (
    echo ERROR compiling index.cpp
    pause
    exit /b 1
)
echo [OK] index.cpp

g++ -std=c++17 -I include -c src/file_scanner.cpp -o build/file_scanner.o 2>&1
if errorlevel 1 (
    echo ERROR compiling file_scanner.cpp
    pause
    exit /b 1
)
echo [OK] file_scanner.cpp

g++ -std=c++17 -I include -c src/search.cpp -o build/search.o 2>&1
if errorlevel 1 (
    echo ERROR compiling search.cpp
    pause
    exit /b 1
)
echo [OK] search.cpp

g++ -std=c++17 -I include -c src/main.cpp -o build/main.o 2>&1
if errorlevel 1 (
    echo ERROR compiling main.cpp
    pause
    exit /b 1
)
echo [OK] main.cpp

echo.
echo Linking...
g++ build/tokenizer.o build/util.o build/document_store.o build/index.o build/file_scanner.o build/search.o build/main.o -lstdc++fs -o build/notesearch.exe 2>&1
if errorlevel 1 (
    echo ERROR linking
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build complete! Executable: build\notesearch.exe
pause
