@echo off
REM Quick launcher for NoteSearch GUI
REM This script runs the GUI application from the build directory

if exist build\notesearch_gui.exe (
    echo Launching NoteSearch GUI...
    start "" "build\notesearch_gui.exe"
) else (
    echo ERROR: notesearch_gui.exe not found in build directory!
    echo Please build the project first using build_gui.bat
    pause
    exit /b 1
)
