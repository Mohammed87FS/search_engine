#include "index.hpp"
#include <algorithm>
#include <unordered_map>

namespace notesearch {

void InvertedIndex::index_document(uint32_t doc_id, const std::vector<std::string>& tokens) {
    // Count term frequencies in this document
    std::unordered_map<std::string, uint32_t> term_counts;
    for (const auto& token : tokens) {
        ++term_counts[token];
    }
    
    // Add postings to the index
    for (const auto& pair : term_counts) {
        const std::string& term = pair.first;
        uint32_t freq = pair.second;
        index_[term].emplace_back(doc_id, freq);
    }
}

const std::vector<Posting>* InvertedIndex::get_postings(const std::string& term) const {
    auto it = index_.find(term);
    if (it != index_.end()) {
        return &(it->second);
    }
    return nullptr;
}

size_t InvertedIndex::get_document_frequency(const std::string& term) const {
    auto it = index_.find(term);
    if (it != index_.end()) {
        return it->second.size();
    }
    return 0;
}

void InvertedIndex::clear() noexcept {
    index_.clear();
}

std::vector<std::string> InvertedIndex::get_all_terms() const {
    std::vector<std::string> terms;
    terms.reserve(index_.size());
    
    for (const auto& pair : index_) {
        terms.push_back(pair.first);
    }
    
    return terms;
}

} // namespace notesearch
