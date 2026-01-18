@echo off
echo ========================================
echo Building NoteSearch GUI (Step by Step)
echo ========================================
echo.

echo Step 1: Compiling tokenizer.cpp...
g++ -std=c++17 -I include -c src/tokenizer.cpp -o build/tokenizer.o 2>&1
if errorlevel 1 (
    echo [FAILED] tokenizer.cpp
    pause
    exit /b 1
)
echo [OK] tokenizer.cpp

echo Step 2: Compiling util.cpp...
g++ -std=c++17 -I include -c src/util.cpp -o build/util.o 2>&1
if errorlevel 1 (
    echo [FAILED] util.cpp
    pause
    exit /b 1
)
echo [OK] util.cpp

echo Step 3: Compiling document_store.cpp...
g++ -std=c++17 -I include -c src/document_store.cpp -o build/document_store.o 2>&1
if errorlevel 1 (
    echo [FAILED] document_store.cpp
    pause
    exit /b 1
)
echo [OK] document_store.cpp

echo Step 4: Compiling index.cpp...
g++ -std=c++17 -I include -c src/index.cpp -o build/index.o 2>&1
if errorlevel 1 (
    echo [FAILED] index.cpp
    pause
    exit /b 1
)
echo [OK] index.cpp

echo Step 5: Compiling file_scanner.cpp...
g++ -std=c++17 -I include -c src/file_scanner.cpp -o build/file_scanner.o 2>&1
if errorlevel 1 (
    echo [FAILED] file_scanner.cpp
    pause
    exit /b 1
)
echo [OK] file_scanner.cpp

echo Step 6: Compiling search.cpp...
g++ -std=c++17 -I include -c src/search.cpp -o build/search.o 2>&1
if errorlevel 1 (
    echo [FAILED] search.cpp
    pause
    exit /b 1
)
echo [OK] search.cpp

echo Step 7: Compiling main_gui.cpp...
g++ -std=c++17 -I include -c src/main_gui.cpp -o build/main_gui.o 2>&1
if errorlevel 1 (
    echo [FAILED] main_gui.cpp
    echo.
    echo Trying with verbose output to see errors...
    g++ -std=c++17 -I include -c src/main_gui.cpp -o build/main_gui.o -v 2>&1 | findstr /C:"error" /C:"Error" /C:"ERROR" /C:"fatal"
    pause
    exit /b 1
)
echo [OK] main_gui.cpp

echo.
echo Step 8: Linking all object files...
g++ build/tokenizer.o build/util.o build/document_store.o build/index.o build/file_scanner.o build/search.o build/main_gui.o -lstdc++fs -lshell32 -lcomdlg32 -lole32 -mwindows -o build/notesearch_gui.exe 2>&1
if errorlevel 1 (
    echo [FAILED] Linking
    echo.
    echo Trying without -mwindows flag...
    g++ build/tokenizer.o build/util.o build/document_store.o build/index.o build/file_scanner.o build/search.o build/main_gui.o -lstdc++fs -lshell32 -lcomdlg32 -lole32 -o build/notesearch_gui.exe 2>&1
    if errorlevel 1 (
        echo [FAILED] Linking without -mwindows
        pause
        exit /b 1
    )
    echo [OK] Linked (with console window)
)

if exist build\notesearch_gui.exe (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Executable: build\notesearch_gui.exe
    dir build\notesearch_gui.exe
    echo.
    echo Launching GUI...
    start "" build\notesearch_gui.exe
) else (
    echo.
    echo ERROR: Executable not created
    pause
    exit /b 1
)

pause
