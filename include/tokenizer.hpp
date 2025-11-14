#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string> 
#include <vector>


namespace notesearch {

    /**
    tokenizes the text into normalized search term, 

    rules
    - Split on whitespace or non alphanumeric characters
    - Convert to lowercase
    - Remove tokens shorter than 2 chars

     @param text The text to tokenize.
     @return A vector of strings.
     */
     std::vector<std::string> tokenize(const std::string& text);

}

#endif