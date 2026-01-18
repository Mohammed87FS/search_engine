@echo off
echo ========================================
echo Building NoteSearch GUI
echo ========================================
echo.

REM Set PATH to include MSYS2 compiler binaries
setlocal
set "PATH=C:\msys64\ucrt64\bin;%PATH%"

echo Compiling all files...
g++ -std=c++17 -O2 -I include ^
    src/tokenizer.cpp ^
    src/util.cpp ^
    src/document_store.cpp ^
    src/index.cpp ^
    src/file_scanner.cpp ^
    src/search.cpp ^
    src/main_gui.cpp ^
    -lstdc++fs ^
    -lshell32 ^
    -lcomdlg32 ^
    -lole32 ^
    -mwindows ^
    -o build/notesearch_gui.exe

if %ERRORLEVEL% EQU 0 (
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
        echo ERROR: Executable not created despite success code
    )
) else (
    echo.
    echo Build failed. Trying without -mwindows flag...
    g++ -std=c++17 -O2 -I include ^
        src/tokenizer.cpp ^
        src/util.cpp ^
        src/document_store.cpp ^
        src/index.cpp ^
        src/file_scanner.cpp ^
        src/search.cpp ^
        src/main_gui.cpp ^
        -lstdc++fs ^
        -lshell32 ^
        -lcomdlg32 ^
        -lole32 ^
        -o build/notesearch_gui.exe
    
    if %ERRORLEVEL% EQU 0 (
        if exist build\notesearch_gui.exe (
            echo.
            echo BUILD SUCCESSFUL (with console window)!
            echo.
            start "" build\notesearch_gui.exe
        )
    ) else (
        echo Build failed completely.
        pause
        exit /b 1
    )
)

pause
