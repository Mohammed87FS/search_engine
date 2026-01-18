#include "tokenizer.hpp"
#include <cctype>

namespace notesearch {

    std::vector<std::string> tokenize(const std::string &text)
    {
        std::vector<std::string> tokens;
        tokens.reserve(text.size() / 4);  // rough estimate
        std::string current_token;
        current_token.reserve(16);
        
        for (char c : text) {
            if (std::isalnum(c)) {
                current_token += std::tolower(static_cast<unsigned char>(c));
            } else {
                // delimiter found
                if (current_token.length() >= 2) {
                    tokens.emplace_back(std::move(current_token));
                    current_token.clear();
                } else {
                    current_token.clear();  // skip short tokens
                }
            }
        }
        
        // handle last token
        if (current_token.length() >= 2) {
            tokens.emplace_back(std::move(current_token));
        }
        
        return tokens;
    }
} 