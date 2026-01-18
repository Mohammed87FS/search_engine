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

namespace notesearch {

void print_usage(const char* program_name) {
    std::cout << "NoteSearch - Local Full-Text Search Engine\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << program_name << " index <directory>    Index a directory\n";
    std::cout << "  " << program_name << " search <query>       Search the index\n";
    std::cout << "  " << program_name << " interactive          Interactive search mode\n";
    std::cout << "\n";
}

void print_results(const std::vector<SearchResult>& results) {
    if (results.empty()) {
        std::cout << "No results found.\n";
        return;
    }
    
    std::cout << "\nFound " << results.size() << " result(s):\n\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        std::cout << "[" << (i + 1) << "] " << result.path << "\n";
        std::cout << "    Score: " << std::fixed << std::setprecision(4) << result.score << "\n";
        if (!result.snippet.empty()) {
            std::cout << "    " << result.snippet << "\n";
        }
        std::cout << "\n";
    }
}

void interactive_mode(DocumentStore& doc_store, InvertedIndex& index) {
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
        auto results = engine.search(query, 10);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        print_results(results);
        std::cout << "Search completed in " << duration.count() << " ms\n\n";
    }
}

} // namespace notesearch

int main(int argc, char* argv[]) {
    using namespace notesearch;
    
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    // static state - would be better to save/load but this works for now
    static DocumentStore doc_store;
    static InvertedIndex index;
    
    if (command == "index") {
        if (argc < 3) {
            std::cerr << "Error: Please provide a directory path.\n";
            return 1;
        }
        
        std::filesystem::path dir_path = argv[2];
        
        std::cout << "Scanning directory: " << dir_path << "\n";
        auto start = std::chrono::high_resolution_clock::now();
        
        FileScanner scanner;
        auto files = scanner.scan_directory(dir_path);
        
        std::cout << "Found " << files.size() << " indexable files.\n";
        std::cout << "Indexing...\n";
        
        doc_store.clear();
        index.clear();
        
        // process each file
        for (const auto& file_pair : files) {
            const auto& file_path = file_pair.first;
            const auto& content = file_pair.second;
            uint32_t doc_id = doc_store.add_document(file_path, content);
            auto tokens = tokenize(content);
            index.index_document(doc_id, tokens);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "\nIndexing complete!\n";
        std::cout << "  Documents indexed: " << doc_store.size() << "\n";
        std::cout << "  Unique terms: " << index.vocabulary_size() << "\n";
        std::cout << "  Time: " << duration.count() << " ms\n";
        
    } else if (command == "search") {
        if (argc < 3) {
            std::cerr << "Error: Please provide a search query.\n";
            return 1;
        }
        
        if (doc_store.empty()) {
            std::cerr << "Error: No documents indexed. Please run 'index' command first.\n";
            return 1;
        }
        
        std::string query = argv[2];
        SearchEngine engine(index, doc_store);
        
        auto start = std::chrono::high_resolution_clock::now();
        auto results = engine.search(query, 10);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        print_results(results);
        std::cout << "Search completed in " << duration.count() << " ms\n";
        
    } else if (command == "interactive") {
        if (doc_store.empty()) {
            std::cerr << "Error: No documents indexed. Please run 'index' command first.\n";
            return 1;
        }
        
        interactive_mode(doc_store, index);
        
    } else {
        std::cerr << "Error: Unknown command '" << command << "'\n\n";
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
