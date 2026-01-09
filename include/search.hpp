#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <string>
#include <vector>
#include <cstdint>
#include "index.hpp"
#include "document_store.hpp"

namespace notesearch {

/**
 * SearchResult represents a single search result
 */
struct SearchResult {
    std::string path;
    double score;
    std::string snippet;
    
    SearchResult(std::string result_path, double result_score, std::string result_snippet)
        : path(std::move(result_path)), score(result_score), snippet(std::move(result_snippet)) {}
};

/**
 * SearchEngine performs queries against the inverted index
 * Implements TF-IDF scoring and multi-term AND queries
 */
class SearchEngine {
public:
    SearchEngine(const InvertedIndex& index, const DocumentStore& doc_store);
    ~SearchEngine() = default;
    
    // Non-copyable, movable
    SearchEngine(const SearchEngine&) = delete;
    SearchEngine& operator=(const SearchEngine&) = delete;
    SearchEngine(SearchEngine&&) noexcept = default;
    SearchEngine& operator=(SearchEngine&&) noexcept = default;
    
    /**
     * Perform a search query
     * @param query The search query string
     * @param max_results Maximum number of results to return (0 = unlimited)
     * @return Vector of search results, sorted by score (descending)
     */
    std::vector<SearchResult> search(const std::string& query, size_t max_results = 10) const;
    
    /**
     * Calculate TF-IDF score for a term in a document
     * @param term The search term
     * @param doc_id Document ID
     * @param total_docs Total number of documents in the index
     * @return TF-IDF score
     */
    double calculate_tf_idf(const std::string& term, uint32_t doc_id, size_t total_docs) const;

private:
    const InvertedIndex& index_;
    const DocumentStore& doc_store_;
    
    /**
     * Calculate term frequency (TF) for a term in a document
     */
    double calculate_tf(const std::string& term, uint32_t doc_id) const;
    
    /**
     * Calculate inverse document frequency (IDF) for a term
     */
    double calculate_idf(const std::string& term, size_t total_docs) const;
    
    /**
     * Extract snippet from document content around matching terms
     */
    std::string extract_snippet(const std::string& content, const std::vector<std::string>& query_terms) const;
};

} // namespace notesearch

#endif // SEARCH_HPP
