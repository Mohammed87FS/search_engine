@echo off
echo Building NoteSearch GUI...
if not exist build mkdir build

REM Set PATH to include MSYS2 compiler binaries (required for DLL dependencies)
setlocal
set "PATH=C:\msys64\ucrt64\bin;%PATH%"

g++ -std=c++17 -O2 -I include src/tokenizer.cpp src/util.cpp src/document_store.cpp src/index.cpp src/file_scanner.cpp src/search.cpp src/main_gui.cpp -lstdc++fs -lshell32 -lcomdlg32 -lole32 -mwindows -o build/notesearch_gui.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo.
    echo Executable: build\notesearch_gui.exe
    echo.
    echo Run the GUI application by double-clicking notesearch_gui.exe
) else (
    echo Build failed. Check errors above.
)
