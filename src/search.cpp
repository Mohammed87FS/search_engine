#include "search.hpp"
#include "tokenizer.hpp"
#include "util.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace notesearch {

SearchEngine::SearchEngine(const InvertedIndex& index, const DocumentStore& doc_store)
    : index_(index), doc_store_(doc_store) {}

std::vector<SearchResult> SearchEngine::search(const std::string& query, size_t max_results) const {
    // Tokenize query
    std::vector<std::string> query_terms = tokenize(query);
    if (query_terms.empty()) {
        return {};
    }
    
    // Remove duplicate terms
    std::unordered_set<std::string> unique_terms(query_terms.begin(), query_terms.end());
    query_terms.assign(unique_terms.begin(), unique_terms.end());
    
    // Find documents containing ALL terms (AND query)
    std::unordered_map<uint32_t, double> doc_scores;
    size_t total_docs = doc_store_.size();
    
    // Start with documents containing the first term
    const auto* first_postings = index_.get_postings(query_terms[0]);
    if (!first_postings) {
        return {}; // No documents match
    }
    
    std::unordered_set<uint32_t> candidate_docs;
    for (const auto& posting : *first_postings) {
        candidate_docs.insert(posting.doc_id);
    }
    
    // Intersect with documents containing other terms
    for (size_t i = 1; i < query_terms.size(); ++i) {
        const auto* postings = index_.get_postings(query_terms[i]);
        if (!postings) {
            return {}; // No documents match all terms
        }
        
        std::unordered_set<uint32_t> term_docs;
        for (const auto& posting : *postings) {
            term_docs.insert(posting.doc_id);
        }
        
        // Intersection: keep only docs in both sets
        std::unordered_set<uint32_t> intersection;
        for (uint32_t doc_id : candidate_docs) {
            if (term_docs.find(doc_id) != term_docs.end()) {
                intersection.insert(doc_id);
            }
        }
        candidate_docs = std::move(intersection);
        
        if (candidate_docs.empty()) {
            return {}; // No documents match all terms
        }
    }
    
    // Calculate TF-IDF scores for matching documents
    for (uint32_t doc_id : candidate_docs) {
        double score = 0.0;
        for (const auto& term : query_terms) {
            score += calculate_tf_idf(term, doc_id, total_docs);
        }
        doc_scores[doc_id] = score;
    }
    
    // Sort by score (descending)
    std::vector<std::pair<uint32_t, double>> sorted_results;
    sorted_results.reserve(doc_scores.size());
    for (const auto& pair : doc_scores) {
        sorted_results.emplace_back(pair.first, pair.second);
    }
    std::sort(sorted_results.begin(), sorted_results.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Build result objects
    std::vector<SearchResult> results;
    size_t result_count = (max_results == 0) ? sorted_results.size() : 
                          std::min(max_results, sorted_results.size());
    results.reserve(result_count);
    
    for (size_t i = 0; i < result_count; ++i) {
        uint32_t doc_id = sorted_results[i].first;
        double score = sorted_results[i].second;
        
        const Document* doc = doc_store_.get_document(doc_id);
        if (doc) {
            std::string snippet = extract_snippet(doc->content, query_terms);
            results.emplace_back(doc->path, score, std::move(snippet));
        }
    }
    
    return results;
}

double SearchEngine::calculate_tf_idf(const std::string& term, uint32_t doc_id, size_t total_docs) const {
    double tf = calculate_tf(term, doc_id);
    double idf = calculate_idf(term, total_docs);
    return tf * idf;
}

double SearchEngine::calculate_tf(const std::string& term, uint32_t doc_id) const {
    const auto* postings = index_.get_postings(term);
    if (!postings) {
        return 0.0;
    }
    
    // Find posting for this document
    for (const auto& posting : *postings) {
        if (posting.doc_id == doc_id) {
            // Use log-normalized TF: 1 + log(tf)
            return 1.0 + std::log(static_cast<double>(posting.term_freq));
        }
    }
    
    return 0.0;
}

double SearchEngine::calculate_idf(const std::string& term, size_t total_docs) const {
    size_t df = index_.get_document_frequency(term);
    if (df == 0 || total_docs == 0) {
        return 0.0;
    }
    
    // IDF = log(N / df)
    return std::log(static_cast<double>(total_docs) / static_cast<double>(df));
}

std::string SearchEngine::extract_snippet(const std::string& content, 
                                          const std::vector<std::string>& query_terms) const {
    if (content.empty() || query_terms.empty()) {
        return "";
    }
    
    // Find first occurrence of any query term
    size_t first_pos = std::string::npos;
    for (const auto& term : query_terms) {
        // Case-insensitive search
        std::string lower_content = content;
        std::transform(lower_content.begin(), lower_content.end(), lower_content.begin(), ::tolower);
        
        std::string lower_term = term;
        std::transform(lower_term.begin(), lower_term.end(), lower_term.begin(), ::tolower);
        
        size_t pos = lower_content.find(lower_term);
        if (pos != std::string::npos && (first_pos == std::string::npos || pos < first_pos)) {
            first_pos = pos;
        }
    }
    
    if (first_pos == std::string::npos) {
        // No match found, return beginning of content
        first_pos = 0;
    }
    
    return notesearch::extract_snippet(content, first_pos, 80);
}

} // namespace notesearch
