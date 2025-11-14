#include "tokenizer.hpp"
#include <cctype>

namespace notesearch {

    std::vector<std::string> tokenize(const std::string &text)
    {
        std::vector<std::string> tokens = {};
        // std::vector<std::string> tokens ; // also possible but the first is more explicit
        std::string current_token;
        
        for (char c : text){ // bewusst nicht const char& c da es cheap ist die copy zu machen 
            if (std::isalnum(c)){
                //static_cast<T>() - compile-time checked conversion
                //dynamic_cast<T>() - runtime checked (for polymorphism)
                //reinterpret_cast<T>() - low-level, type-unsafe conversion
                //const_cast<T>() - add/remove const
                current_token += std::tolower(static_cast<unsigned char>(c)); // the cast ensures that the argument passed to std::tolower is always in the valid, 
                                                                              //non negative range (0-255) specified by the standard, preventing undefined behavior.
            }
            else{
                if (!current_token.length()>=2){
                    tokens.emplace_back(current_token); // emplace_back is generally faster than push_back because it avoids the copy construction of the temporary object.
                }
                current_token.clear(); //Clear the buffer for the next token
            }
            if (current_token.length() >= 2) {
                tokens.emplace_back(current_token);
            }
        }
        return tokens;
    }
}

//This is called Return Value Optimization (RVO) and move semantics. 
//It's automatic 