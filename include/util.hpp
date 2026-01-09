#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <vector>
#include <filesystem>

namespace notesearch {

/**
 * Utility functions for file operations and string manipulation
 */

// Check if a file should be indexed based on extension
bool should_index_file(const std::filesystem::path& file_path);

// Read file content into string (RAII-safe)
std::string read_file_content(const std::filesystem::path& file_path);

// Extract snippet from text around a match position
std::string extract_snippet(const std::string& text, size_t position, size_t context_size = 50);

} // namespace notesearch

#endif // UTIL_HPP
