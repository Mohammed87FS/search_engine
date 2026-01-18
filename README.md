# NoteSearch

A local full-text search engine written in C++

This project implements a search engine that can index directories and search through documents using TF-IDF scoring. It has both a command-line interface and a Windows GUI.

---

## Features

- Index directories recursively
- Search with multi-term queries (AND logic)
- TF-IDF scoring for relevance ranking
- Command-line interface
- Windows GUI (optional)
- No external dependencies (just standard library)

---

## Architecture

The project is split into several components:

1. **FileScanner** - scans directories and reads files
2. **Tokenizer** - splits text into searchable terms
3. **DocumentStore** - stores document paths and content
4. **InvertedIndex** - the main index data structure (term -> documents)
5. **SearchEngine** - handles queries and TF-IDF scoring

---

## Building

### Prerequisites

- C++17 compiler (tested with GCC and MSVC)
- CMake (optional, batch files also work)

### Quick Build (Windows)

```bash
build.bat
```

This creates `build/notesearch.exe` for the CLI version.

For the GUI version:
```bash
build_gui.bat
```

This creates `build/notesearch_gui.exe`.

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

## Usage

### Command Line

Index a directory:
```bash
notesearch index examples
```

Search:
```bash
notesearch search "your query"
```

Interactive mode:
```bash
notesearch interactive
```

### GUI

Just run `notesearch_gui.exe` and use the interface. Click "Index Directory..." to select a folder, then search.

Supported file types: `.txt`, `.md`, `.cpp`, `.hpp`, `.c`, `.h`, `.java`, `.py`, `.js`, and more text-based formats.

---

## Example

```bash
notesearch index examples
notesearch search "search engine"
```

The GUI version is easier to use - just run it and click around.

---

## Notes

This was a project to learn C++ and implement a search engine from scratch. It uses:
- STL containers and algorithms
- Modern C++17 features
- Windows API for the GUI version
- TF-IDF algorithm for ranking results

---

## Performance

Indexing is pretty fast for small to medium directories. Search is usually under 50ms for typical queries.

## Future Improvements

- Save/load index to disk
- Better GUI (maybe add result highlighting)
- Support for more file types
- Maybe add phrase search

## License

Educational use only.
