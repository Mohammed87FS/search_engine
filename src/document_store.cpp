#include "document_store.hpp"
#include <algorithm>

namespace notesearch {

uint32_t DocumentStore::add_document(const std::filesystem::path& file_path, std::string content) {
    uint32_t doc_id = next_id_++;
    documents_.emplace_back(doc_id, file_path.string(), std::move(content));
    return doc_id;
}

const Document* DocumentStore::get_document(uint32_t doc_id) const {
    auto it = std::find_if(documents_.begin(), documents_.end(),
        [doc_id](const Document& doc) { return doc.id == doc_id; });
    
    if (it != documents_.end()) {
        return &(*it);
    }
    return nullptr;
}

void DocumentStore::clear() noexcept {
    documents_.clear();
    next_id_ = 0;
}

} // namespace notesearch
