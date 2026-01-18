# Test-Anleitung

## Testbeispiele erstellt

Die folgenden Testdateien wurden im `examples/` Verzeichnis erstellt:

1. **examples/notes/cpp_basics.txt** - Grundlagen zu C++ und RAII
2. **examples/notes/memory_management.md** - Memory Management Patterns
3. **examples/notes/stl_containers.txt** - STL Container Übersicht
4. **examples/notes/programming_concepts.txt** - Allgemeine Programmierkonzepte
5. **examples/markdown/search_engine.md** - Suchmaschinen-Architektur
6. **examples/code/example.cpp** - C++ Code-Beispiel

## Kompilierung

### Option 1: Mit CMake (empfohlen)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Option 2: Manuell mit g++
```bash
# Windows (PowerShell)
.\build.bat

# Oder manuell:
g++ -std=c++17 -O2 -I include \
    src/tokenizer.cpp src/util.cpp src/document_store.cpp \
    src/index.cpp src/file_scanner.cpp src/search.cpp src/main.cpp \
    -lstdc++fs -o build/notesearch.exe
```

## Testing

Nach erfolgreicher Kompilierung:

### 1. Index erstellen
```bash
build/notesearch.exe index examples
```

Erwartete Ausgabe:
- Anzahl indexierter Dateien
- Anzahl eindeutiger Begriffe
- Indexierungszeit

### 2. Suche durchführen
```bash
build/notesearch.exe search "RAII"
build/notesearch.exe search "memory management"
build/notesearch.exe search "STL containers"
```

### 3. Interaktiver Modus
```bash
build/notesearch.exe interactive
```

Dann können mehrere Suchanfragen eingegeben werden.

## Erwartete Ergebnisse

- **"RAII"** sollte in `cpp_basics.txt` und `memory_management.md` gefunden werden
- **"memory"** sollte in `memory_management.md` und `example.cpp` gefunden werden
- **"STL"** sollte in `stl_containers.txt` gefunden werden
- **"search engine"** sollte in `search_engine.md` gefunden werden

## Troubleshooting

Falls Kompilierung fehlschlägt:
1. Prüfe ob C++17 Compiler installiert ist: `g++ --version`
2. Prüfe ob `std::filesystem` unterstützt wird
3. Auf MinGW: `-lstdc++fs` Flag verwenden
4. Auf MSVC: Automatisch verlinkt
