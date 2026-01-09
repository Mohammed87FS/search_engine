#include "tokenizer.hpp"
#include <cctype>

namespace notesearch {

    std::vector<std::string> tokenize(const std::string &text)
    {
        std::vector<std::string> tokens;
        tokens.reserve(text.size() / 4); // Pre-allocate to reduce reallocations
        std::string current_token;
        current_token.reserve(16); // Pre-allocate typical token size
        
        for (char c : text) {
            if (std::isalnum(c)) {
                current_token += std::tolower(static_cast<unsigned char>(c));
            } else {
                // When we hit a delimiter, save token if it's long enough
                if (current_token.length() >= 2) {
                    tokens.emplace_back(std::move(current_token));
                    current_token.clear();
                } else {
                    current_token.clear();
                }
            }
        }
        
        // Don't forget the last token if text doesn't end with delimiter
        if (current_token.length() >= 2) {
            tokens.emplace_back(std::move(current_token));
        }
        
        return tokens;
    }
}

//This is called Return Value Optimization (RVO) and move semantics. 
//It's automatic 