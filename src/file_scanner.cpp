#include "file_scanner.hpp"
#include "util.hpp"
#include <fstream>
#include <sstream>

namespace notesearch {

std::vector<std::pair<std::filesystem::path, std::string>>
FileScanner::scan_directory(const std::filesystem::path &root_path) const {

  
  std::vector<std::pair<std::filesystem::path, std::string>> files;
  last_stats_ = ScanStats{};
  
  // std::filesystem API 
  if (!std::filesystem::exists(root_path) ||
      !std::filesystem::is_directory(root_path)) {
    // exists() = prüft ob Pfad existiert (Datei oder Ordner)
    // is_directory() = prüft ob Pfad ein Verzeichnis ist (nicht Datei)
  
    return files;
  }
  

  try {
    // try catch .. fängt Exceptions ab
    // Verhindert Programm crash bei Dateisystem Fehlern
    // Code in try Block wird ausgeführt
    // Bei Fehler Sprung zu catchBlock
    
    // recursive_directory_iterator 
    for (const auto &entry : std::filesystem::recursive_directory_iterator(
             root_path,
             std::filesystem::directory_options::skip_permission_denied)) {

      // Durchsucht Verzeichnis und alle Unterverzeichnisse

      
      // directory_options::skip_permission_denied
      // Überspringt Dateien oder Ordner ohne Berechtigung
      // Verhindert Crash bei fehlenden Berechtigungen
    
      
      // entry.is_regular_file() - NEUES KONZEPT!
      if (entry.is_regular_file()) {
        // is_regular_file()  .. prüft ob Eintrag normale Datei ist
        // wenn true .. normale Datei (z.B. .txt, .cpp)
        // false .. Verzeichnis, Symlink, etc.
        // es filtert Verzeichnisse aus
        
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
  } catch (const std::filesystem::filesystem_error &e) {
   log_error("FileScanner: " + std::string(e.what()));
  }
  
  return files;
}

bool FileScanner::should_index(const std::filesystem::path &file_path) const { // kommt ausutil.cpp
  return notesearch::should_index_file(file_path);
}

std::string
FileScanner::read_file(const std::filesystem::path &file_path) const {
  return notesearch::read_file_content(file_path);
}

}