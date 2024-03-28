#ifndef SCANNER
#define SCANNER

#include "token_types.hpp"
#include <vector>
#include <unordered_map>
#include <string>

class Scanner
{

public:
    Scanner(const char* sourceCode);
    void keywordMapInit();
    bool parsePunctuators(const char* c, unsigned int& index, unsigned int& line);
    bool parseIdentifier(const char* c, unsigned int& index, unsigned int& line);
    bool parseConstant(const char* c, unsigned int& index, unsigned int& line);

    bool isDigit(const char& c);
    bool isAlpha(const char& c);
    bool isAlphaDigitFloor(const char& c);
    Token getCurrentToken();
    void incrementTokenId();
    Token popToken();
    void consume(TokenType type);
private:
    std::vector<Token> tokenStream;
    unsigned int currentToken;
    std::unordered_map<std::string, TokenType> keywordMap;
};

#endif