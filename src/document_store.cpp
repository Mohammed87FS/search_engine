#include "document_store.hpp"
#include <algorithm>

namespace notesearch {


uint32_t DocumentStore::add_document(const std::filesystem::path& file_path, std::string content) {
    
    
    uint32_t doc_id = next_id_++;
    // next_id_++ ... post increment, gibt aktuellen Wert zurück, dann erhöht
    // doc_id bekommt zb 0, dann wird next_id_ zu 1
    //   pre increment ist dann
    //   doc_id = ++next_id_   --- > next_id_ wird zuerst erhöht, dann zugewiesen
    //   Beispiel: next_id_ = 0
    //   doc_id = ++next_id_  ---- > next_id_ = 1, doc_id = 1 (beide 1)
    documents_.emplace_back(doc_id, file_path.string(), std::move(content));

    // konstruiert Document mit .. (doc_id, path_string, moved_content)
    
    return doc_id;
    // gibt die zugewiesene document id zurück
}

const Document* DocumentStore::get_document(uint32_t doc_id) const {
   
    // das return ist const Document* .. also Pointer auf konstantes Document (oder nullptr), wenn nicht gefunden
    
    auto it = std::find_if(documents_.begin(), documents_.end(),
        [doc_id](const Document& doc) { return doc.id == doc_id; });
    // std::find_if() = sucht erstes Element das Bedingung erfüllt
    // documents_.begin() = Start-Iterator (Anfang des Vektors)
    // documents_.end() = End-Iterator (nach letztem Element)
    // [doc_id] ... erfasst doc_id aus äußerem Scope .. ist eine lambda function 
    // (const Document& doc) ist Lambda param --> jedes Document im Vektor

    
    if (it != documents_.end()) {
        // iterator vergleich 
        // it != documents_.end() bedeutet.. Element wurde gefunden
        // it == documents_.end() bedeutet.. Nicht gefunden
        
        // Warum nicht it == nullptr?
        // weil iteratoren sind keine Pointer..
        //  end() ist spezieller Iterator Wert für nicht gefunden
        return &(*it); // dereferenziert iterator und gibt pointer auf document zurück
        // Ein pointer speichert die Adresse eines Wertes im Speicher. 
        // Wenn mann einen pointer dereferenzierst, erhält mann den Wert an dieser Adresse.
    }
    return nullptr;
    // Dokument nicht gefunden also nullptr zurückgeben
}

void DocumentStore::clear() noexcept {
    // noexcept garantiert dass keine Exception geworfen wird
    // da clear keine exception werfen soll, da es eine einfache operation ist

    
    documents_.clear();
    // - clear() = entfernt alle Elemente aus dem Vektor    
    next_id_ = 0;

}

} 
