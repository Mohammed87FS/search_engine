#include "index.hpp"
#include <algorithm>
#include <unordered_map>

namespace notesearch {

// Fügt ein Dokument zum Index hinzu
// doc_id = ID des Dokuments
// tokens = Liste aller Wörter aus dem Dokument
void InvertedIndex::index_document(uint32_t doc_id, const std::vector<std::string>& tokens) {
    //  Zähle wie oft jedes Wort vorkommt
    std::unordered_map<std::string, uint32_t> term_counts;
    for (const auto& token : tokens) {
        ++term_counts[token];  // Erhöht Zähler für jedes Wort
    }
    
    // Schritt 2: Füge jedes Wort zum Index hinzu
    for (const auto& pair : term_counts) {
        const std::string& term = pair.first;      // Das Wort
        uint32_t freq = pair.second;                // Wie oft es vorkommt
        index_[term].emplace_back(doc_id, freq);   // Speichere: Wort -> (Dokument-ID, Häufigkeit)
    }
}

// Sucht ein Wort im Index und gibt alle Dokumente zurück, die es enthalten
// term = das gesuchte Wort
// Rückgabe: Pointer auf Liste von Postings (oder nullptr wenn nicht gefunden)
const std::vector<Posting>* InvertedIndex::get_postings(const std::string& term) const {
    auto it = index_.find(term);  // Suche das Wort
    if (it != index_.end()) {
        return &(it->second);      // Gefunden: gib Liste zurück
    }
    return nullptr;                // Nicht gefunden
}

// Gibt zurück: In wie vielen Dokumenten kommt das Wort vor?
// term = das gesuchte Wort
// Rückgabe: Anzahl der Dokumente (0 wenn nicht gefunden)
size_t InvertedIndex::get_document_frequency(const std::string& term) const {
    auto it = index_.find(term);
    if (it != index_.end()) {
        return it->second.size();  // Größe der Liste = Anzahl Dokumente
    }
    return 0;
}

// Löscht den kompletten Index
void InvertedIndex::clear() noexcept {
    index_.clear();
}

// Gibt alle Wörter zurück, die im Index sind
// Nützlich für Debugging oder Auto-Complete
std::vector<std::string> InvertedIndex::get_all_terms() const {
    std::vector<std::string> terms;
    terms.reserve(index_.size());  // Reserviere Speicher für bessere Performance
    
    // Gehe durch alle Einträge im Index
    for (const auto& pair : index_) {
        terms.push_back(pair.first);  // Füge das Wort hinzu
    }
    
    return terms;
}

} 