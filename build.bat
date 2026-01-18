@echo off
echo Building NoteSearch...
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

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo.
    echo To test:
    echo   build\notesearch.exe index examples
    echo   build\notesearch.exe search "RAII"
) else (
    echo Build failed. Check errors above.
)
