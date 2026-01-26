#include "search.hpp"
#include "tokenizer.hpp"
#include "util.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace notesearch {

// Konstruktor: Speichert Referenzen auf Index und DocumentStore
SearchEngine::SearchEngine(const InvertedIndex& index, const DocumentStore& doc_store)
    : index_(index), doc_store_(doc_store) {}

// Hauptsuchfunktion: Sucht nach Query und gibt sortierte Ergebnisse zurück
// query = Suchbegriff (kann mehrere Wörter enthalten)
// max_results = maximale Anzahl Ergebnisse (0 = alle)
std::vector<SearchResult> SearchEngine::search(const std::string& query, size_t max_results) const {
    // Schritt 1: Zerlege Query in einzelne Wörter
    std::vector<std::string> query_terms = tokenize(query);
    if (query_terms.empty()) {
        return {};  // Leere Query = keine Ergebnisse
    }
    
    // Schritt 2: Entferne doppelte Wörter
    std::unordered_set<std::string> unique_terms(query_terms.begin(), query_terms.end());
    query_terms.assign(unique_terms.begin(), unique_terms.end());
    
    // Schritt 3: AND-Query - finde Dokumente die ALLE Wörter enthalten
    std::unordered_map<uint32_t, double> doc_scores;  // Speichert Score für jedes Dokument
    size_t total_docs = doc_store_.size();
    
    // Starte mit dem ersten Wort
    const auto* first_postings = index_.get_postings(query_terms[0]);
    if (!first_postings) {
        return {};  // Erstes Wort nicht gefunden = keine Ergebnisse
    }
    
    // Sammle alle Dokumente die das erste Wort enthalten
    std::unordered_set<uint32_t> candidate_docs;
    for (const auto& posting : *first_postings) {
        candidate_docs.insert(posting.doc_id);
    }
    
    // Schritt 4: Schneide mit anderen Wörtern (Intersection)
    // Nur Dokumente die ALLE Wörter enthalten bleiben übrig
    for (size_t i = 1; i < query_terms.size(); ++i) {
        const auto* postings = index_.get_postings(query_terms[i]);
        if (!postings) {
            return {}; // Wort nicht gefunden = keine Dokumente enthalten alle Wörter
        }
        
        // Sammle Dokumente die dieses Wort enthalten
        std::unordered_set<uint32_t> term_docs;
        for (const auto& posting : *postings) {
            term_docs.insert(posting.doc_id);
        }
        
        // Intersection: Behalte nur Dokumente die in BEIDEN Sets sind
        std::unordered_set<uint32_t> intersection;
        for (uint32_t doc_id : candidate_docs) {
            if (term_docs.find(doc_id) != term_docs.end()) {
                intersection.insert(doc_id);
            }
        }
        candidate_docs = std::move(intersection);
        
        if (candidate_docs.empty()) {
            return {};  // Keine Dokumente enthalten alle Wörter
        }
    }
    
    // Schritt 5: Berechne TF-IDF Score für jedes Dokument
    for (uint32_t doc_id : candidate_docs) {
        double score = 0.0;
        for (const auto& term : query_terms) {
            score += calculate_tf_idf(term, doc_id, total_docs);  // Summiere Scores aller Wörter
        }
        doc_scores[doc_id] = score;
    }
    
    // Schritt 6: Sortiere nach Score (höchster zuerst)
    std::vector<std::pair<uint32_t, double>> sorted_results;
    sorted_results.reserve(doc_scores.size());
    for (const auto& pair : doc_scores) {
        sorted_results.emplace_back(pair.first, pair.second);
    }
    std::sort(sorted_results.begin(), sorted_results.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });  // Absteigend sortieren
    
    // Schritt 7: Baue Ergebnis-Liste mit Snippets
    std::vector<SearchResult> results;
    size_t result_count = (max_results == 0) ? sorted_results.size() : 
                          std::min(max_results, sorted_results.size());
    results.reserve(result_count);
    
    for (size_t i = 0; i < result_count; ++i) {
        uint32_t doc_id = sorted_results[i].first;
        double score = sorted_results[i].second;
        
        const Document* doc = doc_store_.get_document(doc_id);
        if (doc) {
            std::string snippet = extract_snippet(doc->content, query_terms);  // Extrahiere Textausschnitt
            results.emplace_back(doc->path, score, std::move(snippet));
        }
    }
    
    return results;
}

// Berechnet TF-IDF Score für ein Wort in einem Dokument
// TF-IDF = Term Frequency * Inverse Document Frequency
// Höherer Score = Wort ist wichtiger für dieses Dokument
double SearchEngine::calculate_tf_idf(const std::string& term, uint32_t doc_id, size_t total_docs) const {
    double tf = calculate_tf(term, doc_id);      // Wie oft kommt Wort im Dokument vor?
    double idf = calculate_idf(term, total_docs); // Wie selten ist das Wort insgesamt?
    return tf * idf;  // Kombiniert beide Werte
}

// Term Frequency: Wie oft kommt das Wort im Dokument vor?
// Verwendet log-Normalisierung: 1 + log(Häufigkeit)
// Warum log? Häufige Wörter sollen nicht zu dominant werden
double SearchEngine::calculate_tf(const std::string& term, uint32_t doc_id) const {
    const auto* postings = index_.get_postings(term);
    if (!postings) {
        return 0.0;  // Wort nicht gefunden
    }
    
    // Suche das Posting für dieses Dokument
    for (const auto& posting : *postings) {
        if (posting.doc_id == doc_id) {
            // Log-Normalisierung: 1 + log(Häufigkeit)
            // Beispiel: 10x vorkommen → 1 + log(10) ≈ 3.3
            return 1.0 + std::log(static_cast<double>(posting.term_freq));
        }
    }
    
    return 0.0;  // Dokument enthält Wort nicht
}

// Inverse Document Frequency: Wie selten ist das Wort?
// Seltene Wörter = höherer IDF = wichtiger für Suche
// Formel: log(Gesamtanzahl Dokumente / Anzahl Dokumente mit diesem Wort)
double SearchEngine::calculate_idf(const std::string& term, size_t total_docs) const {
    size_t df = index_.get_document_frequency(term);  // In wie vielen Dokumenten kommt Wort vor?
    if (df == 0 || total_docs == 0) {
        return 0.0;
    }
    
    // Standard IDF Formel
    // Beispiel: 100 Dokumente total, Wort in 5 Dokumenten
    // IDF = log(100/5) = log(20) ≈ 3.0
    return std::log(static_cast<double>(total_docs) / static_cast<double>(df));
}

// Extrahiert einen Textausschnitt (Snippet) aus dem Dokument
// Zeigt den Bereich um das erste Vorkommen der Suchwörter
std::string SearchEngine::extract_snippet(const std::string& content, 
                                          const std::vector<std::string>& query_terms) const {
    if (content.empty() || query_terms.empty()) {
        return "";
    }
    
    // Finde erste Position wo eines der Suchwörter vorkommt
    size_t first_pos = std::string::npos;
    for (const auto& term : query_terms) {
        // Case-insensitive Suche
        std::string lower_content = content;
        std::transform(lower_content.begin(), lower_content.end(), lower_content.begin(), ::tolower);
        
        std::string lower_term = term;
        std::transform(lower_term.begin(), lower_term.end(), lower_term.begin(), ::tolower);
        
        size_t pos = lower_content.find(lower_term);
        if (pos != std::string::npos && (first_pos == std::string::npos || pos < first_pos)) {
            first_pos = pos;  // Merke früheste Position
        }
    }
    
    if (first_pos == std::string::npos) {
        first_pos = 0;  // Kein Match gefunden, zeige Anfang
    }
    
    // Nutze Utility-Funktion um Snippet zu extrahieren (80 Zeichen um Position)
    return notesearch::extract_snippet(content, first_pos, 80);
}

} 