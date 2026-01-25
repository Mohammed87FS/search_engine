#include "util.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace notesearch {

//  Prüft ob Datei indexierbar ist basierend auf Endung

bool should_index_file(const std::filesystem::path& file_path) {

    
    const std::string ext = file_path.extension().string();

    
    std::string lower_ext = ext;

    std::transform(lower_ext.begin(), lower_ext.end(), lower_ext.begin(), ::tolower);
    // std::transform() 
    // Transformiert jedes Element im Bereich
    //
    // Parameter:
    //  lower_ext.begin() ... Anfang des Strings
    //  lower_ext.end() ... Ende des Strings
    //  lower_ext.begin() ... wo Ergebnis gespeichert wird

    //  ::tolower Konvertiert jeden Charakter zu lowercase
    // hier wird Zeichen für Zeichen transformiert  und sofort überschrieben. Es wird nicht erst ein neuer String erstellt.
    
    //  Liste der indexierbaren Datei Endungen
    const std::vector<std::string> indexable_extensions = {
        ".txt", ".md", ".markdown", ".cpp", ".hpp", ".c", ".h",
        ".java", ".py", ".js", ".ts", ".rs", ".go", ".rb", ".php",
        ".json", ".xml", ".yaml", ".yml", ".toml", ".ini", ".cfg"
    };

    // std::find() .. sucht Element in einem Container
    return std::find(indexable_extensions.begin(), indexable_extensions.end(), lower_ext) 
           != indexable_extensions.end();
}


// read_file_content: Liest Datei Inhalt sicher (RAII)
std::string read_file_content(const std::filesystem::path& file_path) {
    // Parameter .. Dateipfad zum Lesen
    // Return ..  Datei Inhalt als String
    

    std::ifstream file(file_path, std::ios::binary);
    // std::ifstream .. stream heisst ein Datenstream, der Dateien lesen kann
    // RAII.. Datei wird automatisch im Destruktor geschlossen
    // Auch bei Exception wird Datei geschlossen
    // Der Grund fur die verwendung von RAII (resource acquisition is initialization) ist, dass die Datei automatisch geschlossen wird, wenn das Objekt aus dem Scope heraus geht.
    // das ist wichtig, um Ressourcen freizugeben und um Fehler zu vermeiden.
   
    // Params
    //    Liest Datei als Binär (keine Text Konvertierung)
    //    Wichtig für Unicode, spezielle Zeichen, ...
    //    Ohne binary: also Text Mode
    
    if (!file.is_open()) {
        // Prüft ob Datei erfolgreich geöffnet wurde
   
        return "";

    }
    
    // std::ostringstream - NEUES KONZEPT!
    std::ostringstream buffer;
    // std::ostringstream = Output String Stream
    // String Buffer zum Schreiben (wie std::cout, aber in String)
    // Kann mit << Operator beschrieben werden (<< ist der Insertion Operator)
    // .str() gibt String zurück
    

    buffer << file.rdbuf();
    // rdbuf().. ist Read Buffer 
    // Gibt Pointer auf internen Stream Buffer zurück
    // << Operator kopiert gesamten Buffer in ostringstream
    // Liest Datei komplett in einen Zug 

    // Warum rdbuf()?
    // Schneller (einmaliger Buffer Copy)
    // Behält Formatierung (inkl Zeilenumbrüche)
    
    return buffer.str();
    // .str() = gibt String-Inhalt zurück
    // Konvertiert ostringstream zu std::string
    //  Gibt gesamten Datei-Inhalt zurück
    //
    // RAII file wird automatisch geschlossen (auch bei Exception)
}


// extract_snippet.. Extrahiert Text Ausschnitt um Position herum
std::string extract_snippet(const std::string& text, size_t position, size_t context_size) {
    // Params
    //  const std::string& text ...  Original Text
    // size_t position = Position im Text (wo Match gefunden wurde)
    // size_t context_size = Anzahl Zeichen links/rechts von Position
    //
    // Return
    // Text Ausschnitt mit Context
    // Mit "..." wenn Text weitergeht
    
    if (text.empty() || position >= text.size()) {
        // Prüft ob Text leer oder Position ungültig
        // → empty() = true wenn String Länge 0
        // → position >= text.size() = Position außerhalb des Textes
        return "";
  
    }
    
 
    size_t start = (position > context_size) ? position - context_size : 0;
    // Ternary Operator ? ist ein Bedingter Operator 


    //   Wenn position > context_size:
    //     start = position - context_size (links also)
    //   Sonst:
    //     start = 0 (Beginnt am Tex Anfang)

    

    size_t end = std::min(position + context_size, text.size());
    // std::min() = gibt kleineren Wert zurück .. Verhindert Out of Bounds (end kann nicht > text.size() sein)
    
    // extrahiert den textausschnitt aus dem original text
    std::string snippet = text.substr(start, end - start);
   
    
    // "..." fügt an den Anfang oder Ende des Snippets hinzu, wenn es nicht am Anfang oder Ende des Textes ist
    if (start > 0) {
       
        snippet = "..." + snippet;

    }
    if (end < text.size()) {
      
        snippet = snippet + "..."; // string concatenation

    }
    
    return snippet;

}

} 