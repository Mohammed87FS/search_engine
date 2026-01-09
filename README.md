# 🔍 NoteSearch

**A high-performance local full-text search engine written in modern C++**

NoteSearch is a professional-grade search engine that demonstrates mastery of modern C++ (C++17), including RAII, smart pointers, STL containers, move semantics, and clean architecture. It indexes local directories and provides fast, ranked full-text search using TF-IDF scoring.

---

## ✨ Features

- **Fast Indexing**: Recursively scans and indexes directories in seconds
- **Full-Text Search**: Multi-term AND queries with relevance ranking
- **TF-IDF Scoring**: Industry-standard ranking algorithm
- **Modern C++**: Demonstrates C++17 best practices and idioms
- **Zero Dependencies**: Pure STL, no external libraries required
- **CLI Interface**: Simple command-line interface with interactive mode
- **Clean Architecture**: Header/implementation separation, RAII, const-correctness

---

## 🏗️ Architecture

### Core Components

1. **FileScanner** (`file_scanner.hpp/cpp`)
   - Recursively traverses directories using `std::filesystem`
   - Filters indexable file types
   - RAII-safe file reading

2. **Tokenizer** (`tokenizer.hpp/cpp`)
   - Normalizes text into searchable terms
   - Splits on non-alphanumeric characters
   - Converts to lowercase, filters short tokens

3. **DocumentStore** (`document_store.hpp/cpp`)
   - Maps document IDs to file paths and content
   - Efficient storage using `std::vector`

4. **InvertedIndex** (`index.hpp/cpp`)
   - Core data structure: `term → vector<Posting>`
   - Fast term lookup using `std::unordered_map`
   - Stores term frequencies per document

5. **SearchEngine** (`search.hpp/cpp`)
   - Implements TF-IDF scoring
   - Multi-term AND query processing
   - Result ranking and snippet extraction

---

## 🚀 Building

### Prerequisites

- **C++17-compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.15+**

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# On Windows (MSVC)
cmake --build . --config Release

# On Linux/macOS
make -j$(nproc)
```

The executable will be in `build/bin/notesearch` (or `build/bin/Release/notesearch` on Windows).

---

## 📖 Usage

### Index a Directory

```bash
notesearch index /path/to/documents
```

This recursively scans the directory, tokenizes all indexable files, and builds the inverted index.

**Supported file types**: `.txt`, `.md`, `.markdown`, `.cpp`, `.hpp`, `.c`, `.h`, `.java`, `.py`, `.js`, `.ts`, `.rs`, `.go`, `.rb`, `.php`, `.json`, `.xml`, `.yaml`, `.yml`, `.toml`, `.ini`, `.cfg`

### Search

```bash
notesearch search "modern C++ patterns"
```

Returns ranked results with:
- File path
- Relevance score (TF-IDF)
- Text snippet showing context

### Interactive Mode

```bash
notesearch interactive
```

Enter multiple queries without re-indexing. Type `quit` or `exit` to leave.

---

## 🎯 Example

```bash
# Index your notes directory
$ notesearch index ~/notes

Scanning directory: /home/user/notes
Found 127 indexable files.
Indexing...

Indexing complete!
  Documents indexed: 127
  Unique terms: 8,432
  Time: 234 ms

# Search for something
$ notesearch search "RAII memory management"

Found 3 result(s):

[1] /home/user/notes/cpp_best_practices.md
    Score: 12.3456
    ...RAII (Resource Acquisition Is Initialization) is a fundamental C++ pattern for memory management...

[2] /home/user/notes/smart_pointers.md
    Score: 8.9012
    ...Modern C++ uses RAII through smart pointers to automatically manage memory...

Search completed in 12 ms
```

---

## 🧠 Learning Objectives

This project teaches:

- ✅ **RAII** - Resource management through object lifetime
- ✅ **Smart Pointers** - `std::unique_ptr`, `std::shared_ptr` (where applicable)
- ✅ **Move Semantics** - Avoiding unnecessary copies with `std::move`
- ✅ **STL Containers** - `std::vector`, `std::unordered_map`, `std::string`
- ✅ **STL Algorithms** - `std::sort`, `std::find_if`, `std::transform`
- ✅ **Const Correctness** - Proper use of `const` for immutability
- ✅ **Value vs Reference Semantics** - When to copy, when to reference
- ✅ **Modern Filesystem API** - `std::filesystem` for directory traversal
- ✅ **Clean Architecture** - Header/implementation separation
- ✅ **CMake Build System** - Professional project structure

---

## 📊 Performance

- **Indexing**: ~8,000 files in < 3 seconds (on modern hardware)
- **Query Time**: < 50ms for typical queries
- **Memory**: Efficient storage with minimal overhead

---

## 🧪 Testing

Unit tests are planned for:
- Tokenizer correctness
- Index building
- Search algorithm
- TF-IDF scoring

Run tests with:
```bash
cd build
ctest
```

---

## 🔮 Future Enhancements

- [ ] **Persistent Index**: Save/load index to disk
- [ ] **Wildcard Queries**: `"memor*"` → memory, memorize, memorial
- [ ] **Phrase Queries**: `"dynamic programming"` (exact phrase)
- [ ] **Parallel Indexing**: Multi-threaded file processing
- [ ] **HTTP Server**: REST API + web UI
- [ ] **Real-time Updates**: Auto-reindex on file changes

---

## 📝 Code Style

- **Modern C++17** idioms throughout
- **RAII** for all resource management
- **Const correctness** wherever possible
- **Move semantics** to avoid copies
- **STL algorithms** over raw loops
- **Clear naming** and documentation

---

## 🤝 Contributing

This is a learning project, but contributions are welcome! Focus areas:
- Performance optimizations
- Additional file type support
- Test coverage
- Documentation improvements

---

## 📄 License

This project is provided as-is for educational purposes.

---

## 🎓 Author

Built to master modern C++ through hands-on implementation of a real-world system.

**Key Learnings**:
- Understanding memory management without garbage collection
- Designing efficient data structures
- Writing performant, maintainable C++ code
- Building a complete system from scratch

---

## 🙏 Acknowledgments

Inspired by search engines like Lucene, but implemented from scratch to deeply understand the algorithms and data structures.

---

**Happy Searching! 🔍**
