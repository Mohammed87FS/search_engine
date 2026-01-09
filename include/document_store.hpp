#ifndef DOCUMENT_STORE_HPP
#define DOCUMENT_STORE_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace notesearch {

/**
 * Represents a single document in the index
 */
struct Document {
    uint32_t id;
    std::string path;
    std::string content;
    
    Document(uint32_t doc_id, std::string doc_path, std::string doc_content)
        : id(doc_id), path(std::move(doc_path)), content(std::move(doc_content)) {}
};

/**
 * DocumentStore manages the collection of all indexed documents
 * Maps document IDs to file paths and content
 */
class DocumentStore {
public:
    DocumentStore() = default;
    ~DocumentStore() = default;
    
    // Non-copyable, movable
    DocumentStore(const DocumentStore&) = delete;
    DocumentStore& operator=(const DocumentStore&) = delete;
    DocumentStore(DocumentStore&&) noexcept = default;
    DocumentStore& operator=(DocumentStore&&) noexcept = default;
    
    /**
     * Add a document to the store
     * @param file_path Path to the file
     * @param content File content
     * @return The assigned document ID
     */
    uint32_t add_document(const std::filesystem::path& file_path, std::string content);
    
    /**
     * Get document by ID
     * @param doc_id Document ID
     * @return Pointer to document, or nullptr if not found
     */
    const Document* get_document(uint32_t doc_id) const;
    
    /**
     * Get total number of documents
     */
    size_t size() const noexcept { return documents_.size(); }
    
    /**
     * Check if store is empty
     */
    bool empty() const noexcept { return documents_.empty(); }
    
    /**
     * Clear all documents
     */
    void clear() noexcept;
    
    /**
     * Get all documents (for iteration)
     */
    const std::vector<Document>& get_all_documents() const noexcept { return documents_; }

private:
    std::vector<Document> documents_;
    uint32_t next_id_ = 0;
};

} // namespace notesearch

#endif // DOCUMENT_STORE_HPP
