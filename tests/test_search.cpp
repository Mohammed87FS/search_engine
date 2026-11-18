// kleine tests ohne framework, laufen gegen notes_examples
// start aus dem repo ordner: build/bin/notesearch_tests.exe  (oder ctest)

#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.hpp"
#include "file_scanner.hpp"
#include "document_store.hpp"
#include "index.hpp"
#include "search.hpp"

using namespace notesearch;

static int failed = 0;

static void check(bool ok, const std::string& name) {
    std::cout << (ok ? "ok    " : "FAIL  ") << name << "\n";
    if (!ok) ++failed;
}

static bool ends_with(const std::string& s, const std::string& end) {
    return s.size() >= end.size() && s.compare(s.size() - end.size(), end.size(), end) == 0;
}

int main() {
    // tokenizer
    auto t = tokenize("Hallo Welt, C++ ist toll! a 42");
    check(t == std::vector<std::string>{"hallo", "welt", "ist", "toll", "42"},
          "tokenize: lowercase, satzzeichen weg, 1-zeichen woerter weg");

    auto u = tokenize("Gr\xC3\xB6\xC3\x9F" "e Messung");   // "Größe" in UTF-8
    check(u.size() == 2 && u[0] == "gr\xC3\xB6\xC3\x9F" "e",
          "tokenize: umlaut bleibt im wort");

    // index ueber die beispiel notizen
    FileScanner scanner;
    auto files = scanner.scan_directory("notes_examples");
    check(files.size() == 3, "scanner findet 3 dateien");

    DocumentStore docs;
    InvertedIndex index;
    for (const auto& f : files) {
        uint32_t id = docs.add_document(f.first, f.second);
        index.index_document(id, tokenize(f.second));
    }

    SearchEngine engine(index, docs);

    auto r = engine.search("programming", 10);
    check(r.size() == 2, "programming: 2 treffer");
    check(!r.empty() && ends_with(r[0].path, "programming_concepts.txt"),
          "programming: programming_concepts.txt zuerst (kommt dort oefter vor)");
    check(r.size() == 2 && r[0].score > r[1].score, "programming: absteigend sortiert");

    auto a = engine.search("smart pointers", 10);
    check(a.size() == 1 && ends_with(a[0].path, "cpp_basics.txt"), "AND query: smart pointers");

    auto n = engine.search("smart inheritance", 10);
    check(n.empty(), "AND query: woerter aus verschiedenen dateien -> nichts");

    check(engine.search("gibtesnicht", 10).empty(), "unbekanntes wort -> nichts");

    std::cout << (failed == 0 ? "\nalle tests ok\n" : "\nfehler: " + std::to_string(failed) + "\n");
    return failed == 0 ? 0 : 1;
}
