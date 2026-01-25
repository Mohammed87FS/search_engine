#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include "tokenizer.hpp"
#include "file_scanner.hpp"
#include "document_store.hpp"
#include "index.hpp"
#include "search.hpp"

// command line interface logik
namespace notesearch {

void print_usage(const char* program_name) { // das stern hier ist asterisk pointer (const char* program_name)
    std::cout << "NoteSearch - Local Full-Text Search Engine\n\n";
    std::cout << "Usage:\n";  // akzeptiert 3 commands.. index, search, interactive
    std::cout << "  " << program_name << " index <directory>    Index a directory\n";
    std::cout << "  " << program_name << " search <query>       Search the index\n";
    std::cout << "  " << program_name << " interactive          Interactive search mode\n";
    std::cout << "\n";
}

void print_results(const std::vector<SearchResult>& results) {
    // print_results ist eine freie Funktion (keine member funktion), member funktionen sind funktionen die zu einer klasse gehören und auf objekte der klasse zugreifen können

    if (results.empty()) {
        std::cout << "No results found.\n";
        return;
    }
    
    std::cout << "\nFound " << results.size() << " result(s):\n\n";
    
    // iteriert über alle Suchergebnisse
    for (size_t i = 0; i < results.size(); ++i) { 
        // size() = member funktion von vector, gibt Anzahl der Elemente zurück
        
        const auto& result = results[i]; 
        // const auto& = konstante Referenz auf SearchResult Objekt (wichtig ->> keine Kopie)
        
        std::cout << "[" << (i + 1) << "] " << result.path << "\n";
        
        // Score = TF-IDF Relevanz-Wert (nicht nur Häufigkeit!)
        // TF IDF = (Term Frequency) × (Inverse Document Frequency)
        // Berücksichtigt --->> Häufigkeit im Dokument UND Seltenheit insgesamt (also ein wichtiges wort in einem dokument, kommt im dokument viel vor, aber selten im gesamten corpus, "the" ist ein unwichtiges Wort zb.)
        std::cout << "    Score: " << std::fixed << std::setprecision(4) << result.score << "\n";
        
        // Snippet .. Text Ausschnitt um die gefundene stelle 
        if (!result.snippet.empty()) {
            std::cout << "    " << result.snippet << "\n";
        }
        std::cout << "\n";
    }
}

void interactive_mode(DocumentStore& doc_store, InvertedIndex& index) { // diese parameter sind referenzen auf die document store und index, weil wir sie verändern wollen
    std::cout << "Entering interactive mode. Type 'quit' or 'exit' to exit.\n\n";
    
    std::string query;
    while (true) {
        std::cout << "search> ";
        std::getline(std::cin, query);
        
        if (query == "quit" || query == "exit" || query == "q") {
            break;
        }
        
        if (query.empty()) {
            continue;
        }
        
        if (doc_store.empty()) {
            std::cout << "Error: No documents indexed. Please index a directory first.\n";
            continue;
        }
        
        SearchEngine engine(index, doc_store);
        auto start = std::chrono::high_resolution_clock::now();
        auto results = engine.search(query, 10); // 10 ist max anzahl an ergebnissen die zurückgegeben werden sollen
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        print_results(results);
        std::cout << "Search completed in " << duration.count() << " ms\n\n";
    }
}

} // namespace notesearch

int main(int argc, char* argv[]) {
    using namespace notesearch;
    
    if (argc < 2) {  // um den command zu verarbeiten, muss das programm mindestens 2 argumente haben, also argv[0] und argv[1]
        print_usage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    // static .. would be better to save/load but this works for now
    // das heisst index lebt so lange wie das programm läuft (also im memory)
    static DocumentStore doc_store;
    static InvertedIndex index;
    
    if (command == "index") {
        if (argc < 3) { // argc ist die anzahl der argumente die an das programm übergeben werden
            std::cerr << "Falsch: Bitte einen Pfad zu einem Verzeichnis eingeben!!.\n";
            return 1; // return 1 ist ein fehlercode
        }
        
        std::filesystem::path dir_path = argv[2]; // std::filesystem::path ist ein objekt der klasse std::filesystem::path,
        // argv[2] ist der zweite argument der an das programm übergeben wird
        
        std::cout << "Scanning directory: " << dir_path << "\n";
        auto start = std::chrono::high_resolution_clock::now(); // startet die zeitmessung durch std::chrono::high_resolution_clock::now(), diese gibt die aktuelle zeit in nanosekunden zurück
        
        FileScanner scanner; // scanner ist ein objekt der klasse FileScanner
        auto files = scanner.scan_directory(dir_path); // scanner.scan_directory(dir_path) gibt alle indexierbaren dateien im verzeichnis aus
        
        std::cout << "Found " << files.size() << " indexable files.\n"; // gibt die anzahl der indexierbaren dateien aus
        std::cout << "Indexing...\n";
        
        doc_store.clear();
        index.clear(); // clear ist technisch eine member function der klasse InvertedIndex, die alle postings (dateien die das wort enthalten) und die term frequency entfernt
        
        // Jede Datei im Verzeichnis wird verarbeitet
        for (const auto& file_pair : files) { 
            // const auto& = Referenz (keine Kopie), file_pair ist ein Paar (Pfad, Inhalt)
            
            const auto& file_path = file_pair.first;   // erstes Teil .. Dateipfad
            const auto& content = file_pair.second;     // zweites Teil .. Dateiinhalt 
            
            // weist dem doku eine eindeutige id zu also zb (0, 1, 2...)
            uint32_t doc_id = doc_store.add_document(file_path, content);
            
            // zerlegt text in normalisierte Wörter .. lowercase, min. 2 Zeichen..
            // zb: "Hallo ich bin Mohammed!" --> ["hello", "ich", "bin", "mohammed"] wie ein array von strings
            auto tokens = tokenize(content);
            
            // fügt Wörter zum inverted index hinzu (inverted index ist eine datenstruktur die die wörter und die dazugehörigen dateien speichert, mapping also)
            // Erstellt Mapping... Wort ---->  [Dokumente die dieses Wort enthalten]
            // zb: "gut" hat die dokumente [doc_id=1, doc_id=3]
            index.index_document(doc_id, tokens);
        }
        
        auto end = std::chrono::high_resolution_clock::now(); // endet die zeitmessung
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "\nIndexing complete!\n";
        std::cout << "  Documents indexed: " << doc_store.size() << "\n";
        std::cout << "  Unique terms: " << index.vocabulary_size() << "\n";
        std::cout << "  Time: " << duration.count() << " ms\n";
        
    } else if (command == "search") {
        if (argc < 3) {
            std::cerr << "Falsch: Bitte eine Suchanfrage eingeben!.\n";
            return 1;
        }
        
        if (doc_store.empty()) {
            std::cerr << "Falsch: Keine Dokumente indexiert. Bitte 'index' kommando zuerst ausführen.\n";
            return 1;
        }
        
        std::string query = argv[2]; // TODO  i dont understand why argv[2] is the query ?? answer: because the command is "search <query>" ??
        SearchEngine engine(index, doc_store);
        
        auto start = std::chrono::high_resolution_clock::now();
        auto results = engine.search(query, 10); // 10 is the maximum number of results to return
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        print_results(results);
        std::cout << "Search completed in " << duration.count() << " ms\n";
        
    } else if (command == "interactive") {
        if (doc_store.empty()) { // doc_store.empty() ist true wenn der document store leer ist
            std::cerr << "Falsch: Keine Dokumente indexiert. Bitte 'index' kommando zuerst ausführen.\n";
            return 1;
        }
        
        interactive_mode(doc_store, index);
        
    } else {
        std::cerr << "Falsch: Unbekanntes kommando '" << command << "'\n\n";
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
