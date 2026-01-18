# Quick launcher for NoteSearch GUI (PowerShell)
# This script runs the GUI application from the build directory

$exePath = ".\build\notesearch_gui.exe"

if (Test-Path $exePath) {
    Write-Host "Launching NoteSearch GUI..." -ForegroundColor Green
    & $exePath
} else {
    Write-Host "ERROR: notesearch_gui.exe not found in build directory!" -ForegroundColor Red
    Write-Host "Please build the project first using build_gui.bat" -ForegroundColor Yellow
    exit 1
}
