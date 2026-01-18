#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <string>
#include <vector>
#include <cstdint>
#include "index.hpp"
#include "document_store.hpp"

namespace notesearch {

// search result structure
struct SearchResult {
    std::string path;
    double score;
    std::string snippet;
    
    SearchResult(std::string result_path, double result_score, std::string result_snippet)
        : path(std::move(result_path)), score(result_score), snippet(std::move(result_snippet)) {}
};

// search engine - handles queries and scoring
class SearchEngine {
public:
    SearchEngine(const InvertedIndex& index, const DocumentStore& doc_store);
    ~SearchEngine() = default;
    
    // Non-copyable, movable
    SearchEngine(const SearchEngine&) = delete;
    SearchEngine& operator=(const SearchEngine&) = delete;
    SearchEngine(SearchEngine&&) noexcept = default;
    SearchEngine& operator=(SearchEngine&&) noexcept = default;
    
    // search with max results limit
    std::vector<SearchResult> search(const std::string& query, size_t max_results = 10) const;
    
    // calculate TF-IDF score
    double calculate_tf_idf(const std::string& term, uint32_t doc_id, size_t total_docs) const;

private:
    const InvertedIndex& index_;
    const DocumentStore& doc_store_;
    
    double calculate_tf(const std::string& term, uint32_t doc_id) const;
    double calculate_idf(const std::string& term, size_t total_docs) const;
    std::string extract_snippet(const std::string& content, const std::vector<std::string>& query_terms) const;
};

} // namespace notesearch

#endif // SEARCH_HPP
