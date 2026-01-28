# Search Engine Architecture

## Inverted Index
An inverted index maps terms to documents containing those terms.
This data structure enables fast full-text search.

## TF-IDF Scoring
TF-IDF (Term Frequency - Inverse Document Frequency) ranks documents by relevance.
- TF: How often a term appears in a document
- IDF: How rare a term is across all documents

Higher scores indicate more relevant documents.

## Tokenization
Text is split into normalized tokens for indexing.
Lowercase conversion and filtering improve search quality.
