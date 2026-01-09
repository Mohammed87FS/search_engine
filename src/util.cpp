#include "util.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace notesearch {

bool should_index_file(const std::filesystem::path& file_path) {
    const std::string ext = file_path.extension().string();
    
    // Convert to lowercase for comparison
    std::string lower_ext = ext;
    std::transform(lower_ext.begin(), lower_ext.end(), lower_ext.begin(), ::tolower);
    
    // Indexable extensions
    const std::vector<std::string> indexable_extensions = {
        ".txt", ".md", ".markdown", ".cpp", ".hpp", ".c", ".h",
        ".java", ".py", ".js", ".ts", ".rs", ".go", ".rb", ".php",
        ".json", ".xml", ".yaml", ".yml", ".toml", ".ini", ".cfg"
    };
    
    return std::find(indexable_extensions.begin(), indexable_extensions.end(), lower_ext) 
           != indexable_extensions.end();
}

std::string read_file_content(const std::filesystem::path& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    // Read entire file into string
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string extract_snippet(const std::string& text, size_t position, size_t context_size) {
    if (text.empty() || position >= text.size()) {
        return "";
    }
    
    size_t start = (position > context_size) ? position - context_size : 0;
    size_t end = std::min(position + context_size, text.size());
    
    std::string snippet = text.substr(start, end - start);
    
    // Add ellipsis if not at start/end
    if (start > 0) {
        snippet = "..." + snippet;
    }
    if (end < text.size()) {
        snippet = snippet + "...";
    }
    
    return snippet;
}

} // namespace notesearch
