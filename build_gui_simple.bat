@echo off
echo Building NoteSearch GUI...
g++ -std=c++17 -O2 -I include src/tokenizer.cpp src/util.cpp src/document_store.cpp src/index.cpp src/file_scanner.cpp src/search.cpp src/main_gui.cpp -lstdc++fs -lshell32 -lcomdlg32 -lole32 -mwindows -o build/notesearch_gui.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    build\notesearch_gui.exe
) else (
    echo Build failed with -mwindows, trying without...
    g++ -std=c++17 -O2 -I include src/tokenizer.cpp src/util.cpp src/document_store.cpp src/index.cpp src/file_scanner.cpp src/search.cpp src/main_gui.cpp -lstdc++fs -lshell32 -lcomdlg32 -lole32 -o build/notesearch_gui.exe
    if %ERRORLEVEL% EQU 0 (
        echo Build successful!
        build\notesearch_gui.exe
    )
)
