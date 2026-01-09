#ifndef FILE_SCANNER_HPP
#define FILE_SCANNER_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <utility>

namespace notesearch {

/**
 * FileScanner recursively scans directories and reads file contents
 * Uses RAII and modern filesystem API
 */
class FileScanner {
public:
    FileScanner() = default;
    ~FileScanner() = default;
    
    // Non-copyable, movable
    FileScanner(const FileScanner&) = delete;
    FileScanner& operator=(const FileScanner&) = delete;
    FileScanner(FileScanner&&) noexcept = default;
    FileScanner& operator=(FileScanner&&) noexcept = default;
    
    /**
     * Scan a directory recursively and return all indexable files
     * @param root_path Root directory to scan
     * @return Vector of (file_path, file_content) pairs
     */
    std::vector<std::pair<std::filesystem::path, std::string>> scan_directory(
        const std::filesystem::path& root_path) const;
    
    /**
     * Get statistics about the scan
     */
    struct ScanStats {
        size_t files_scanned = 0;
        size_t files_indexed = 0;
        size_t total_bytes = 0;
    };
    
    ScanStats get_last_scan_stats() const noexcept { return last_stats_; }

private:
    mutable ScanStats last_stats_;
    
    /**
     * Check if a file should be indexed
     */
    bool should_index(const std::filesystem::path& file_path) const;
    
    /**
     * Read file content safely
     */
    std::string read_file(const std::filesystem::path& file_path) const;
};

} // namespace notesearch

#endif // FILE_SCANNER_HPP
