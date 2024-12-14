#ifndef SCANNER_H
#define SCANNER_H

#include "token.hpp"
#include <queue>
#include <unordered_map>
#include <string>

class Scanner
{

public:
    Scanner(const char* sourceCode);
    bool currentTokenOneOf(const std::vector<TokenType>& types);
    bool currentTokenOneOf(const TokenType* types, uint32_t tokenCount);
    Token getToken();
    Token peekToken();
    bool match(TokenType type);
    void consume(TokenType type);
private:
    void keywordMapInit();
    Token parsePunctuators(const char* c);
    Token parseIdentifier(const char* c);
    Token parseConstant(const char* c);

    bool isDigit(const char& c);
    bool isAlpha(const char& c);
    bool isAlphaDigitFloor(const char& c);
private:
    const char* src_buffer;
    std::queue<Token> token_queue;
    unsigned int index;
    unsigned int line;
    std::unordered_map<std::string, TokenType> keywordMap;
};

#endif