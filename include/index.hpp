#ifndef INDEX_HPP
#define INDEX_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "document_store.hpp"

namespace notesearch {

/**
 * Posting represents a single occurrence of a term in a document
 */
struct Posting {
    uint32_t doc_id;       // Document ID
    uint32_t term_freq;    // Frequency of term in this document
    
    Posting(uint32_t id, uint32_t freq) : doc_id(id), term_freq(freq) {}
};

/**
 * InvertedIndex is the core data structure for fast full-text search
 * Maps terms -> list of postings (documents containing the term)
 */
class InvertedIndex {
public:
    InvertedIndex() = default;
    ~InvertedIndex() = default;
    
    // Non-copyable, movable
    InvertedIndex(const InvertedIndex&) = delete;
    InvertedIndex& operator=(const InvertedIndex&) = delete;
    InvertedIndex(InvertedIndex&&) noexcept = default;
    InvertedIndex& operator=(InvertedIndex&&) noexcept = default;
    
    /**
     * Index a document: add all its terms to the inverted index
     * @param doc_id Document ID
     * @param tokens Vector of normalized tokens from the document
     */
    void index_document(uint32_t doc_id, const std::vector<std::string>& tokens);
    
    /**
     * Get postings list for a term
     * @param term The search term
     * @return Pointer to postings list, or nullptr if term not found
     */
    const std::vector<Posting>* get_postings(const std::string& term) const;
    
    /**
     * Get document frequency (number of documents containing the term)
     * @param term The search term
     * @return Document frequency, or 0 if term not found
     */
    size_t get_document_frequency(const std::string& term) const;
    
    /**
     * Get total number of unique terms in the index
     */
    size_t vocabulary_size() const noexcept { return index_.size(); }
    
    /**
     * Check if index is empty
     */
    bool empty() const noexcept { return index_.empty(); }
    
    /**
     * Clear the entire index
     */
    void clear() noexcept;
    
    /**
     * Get all terms (for iteration/debugging)
     */
    std::vector<std::string> get_all_terms() const;

private:
    // term -> vector of postings
    std::unordered_map<std::string, std::vector<Posting>> index_;
};

} // namespace notesearch

#endif // INDEX_HPP
