#include "tokenizer.hpp"
#include <cctype>

namespace notesearch { 
    // mamespace ist container für Funktionen, Variablen also keine Klasse
    // ziel ist verhinderung Namenskonflikte mit anderen Libraries

    std::vector<std::string> tokenize(const std::string &text) {
        // Parameter  .... const std::string &text
        // &  ..... Referenz (keine Kopie, spart Memory)
        
        std::vector<std::string> tokens; 
        // Vector ist ein dynamisches Array von Strings
        
        tokens.reserve(text.size() / 4); 
        // Performance Optimierung..
        // - Schätzung.. ca 25% des Textes sind Tokens
        // - Reserviert Memory im vorhinen .. das vermeidet Reallokationen 
        // - Reallokation ist teuer (kopiert alle Elemente)
        
        std::string current_token;
        
        current_token.reserve(16); 
        // Performance Optimierung wieder
        // - reserviert 16 Zeichen (16 ist eine typische Wortlänge)
        // - vrermeidet Reallokationen auch..
        
        for (char c : text) { 
            // Range based for loop.. iteriert über jeden Charakter
            
            if (std::isalnum(c)) { 
                // Prüft ob Charakter alphanumerisch ist (a to z, A to Z, 0 to9 )
                
                current_token += std::tolower(static_cast<unsigned char>(c)); 
                // - tolower() konvertiert zu lowercase
                // - static_cast<unsigned char> = Cast wegen signed/unsigned char Problem
                //   wichtig weil(verhindert Undefined Behavior bei negativen Werten)
                // - += fügt Charakter zum Token hinzu
                
            } else { 
                // delimiter gefunden (Leerzeichen, Sonderzeichen)
                
                if (current_token.length() >= 2) {
                    // tokem muss lang genug sein(min 2 chars)
                    
                    tokens.emplace_back(std::move(current_token)); 
                    // - emplace_back() = konstruiert direkt im Container (schneller als push_back), das heißt es wird keine neue stelle im memory reserviert und keine elemente kopiert, sondern direkt an die stelle im memory geschrieben.
                    // - std::move() = übergibt Besitz statt Kopie (spart Zeit bei großen Strings)
                    // - current_token ist danach leer 

                    // wenn mann emplace_back() verwendet, wird der Konstruktor des Objekts 
                    // direkt auf dem Platz aufgerufen, der für das neue Element reserviert ist. 
                    // So wird das Objekt in place im Container konstruiert.
                    current_token.clear();
                } else {
                    // Token zu kurz 
                    current_token.clear();
                }
            }
        }
        
        // Handle last token
        // falls Text nicht mit Delimiter endet, ist letztes Token noch in current_token
        if (current_token.length() >= 2) {
            tokens.emplace_back(std::move(current_token));
        }
        
        return tokens; 
    }
}