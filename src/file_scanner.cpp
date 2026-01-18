#include "file_scanner.hpp"
#include "util.hpp"
#include <fstream>
#include <sstream>

namespace notesearch {

std::vector<std::pair<std::filesystem::path, std::string>> 
FileScanner::scan_directory(const std::filesystem::path& root_path) const {
    std::vector<std::pair<std::filesystem::path, std::string>> files;
    last_stats_ = ScanStats{};
    
    if (!std::filesystem::exists(root_path) || !std::filesystem::is_directory(root_path)) {
        return files;
    }
    
    try {
        // recursive iterator - skips permission denied errors
        for (const auto& entry : std::filesystem::recursive_directory_iterator(
                 root_path, std::filesystem::directory_options::skip_permission_denied)) {
            
            if (entry.is_regular_file()) {
                ++last_stats_.files_scanned;
                
                if (should_index(entry.path())) {
                    std::string content = read_file(entry.path());
                    if (!content.empty()) {
                        files.emplace_back(entry.path(), std::move(content));
                        ++last_stats_.files_indexed;
                        last_stats_.total_bytes += content.size();
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // ignore errors for now - could add logging later
    }
    
    return files;
}

bool FileScanner::should_index(const std::filesystem::path& file_path) const {
    return notesearch::should_index_file(file_path);
}

std::string FileScanner::read_file(const std::filesystem::path& file_path) const {
    return notesearch::read_file_content(file_path);
}

} // namespace notesearch
