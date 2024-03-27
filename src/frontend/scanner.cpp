#include "../../include/frontend/scanner.hpp"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

Scanner::Scanner(const char *sourceCode)
:
currentToken(0)
{
    keywordMapInit();

    unsigned int i =0;
    unsigned int line = 0;
    while (sourceCode[i] != '\0')
    {
        if(parsePunctuators(sourceCode, i, line))
        {
            i++;
            continue;
        }
        if(parseIdentifier(sourceCode, i, line))
        {
            i++;
            continue;
        }
        if(sourceCode[i] == '\n')
        {
            line++;
            i++;
            continue;
        }
        i++;
    }
    
    tokenStream.push_back( Token{TokenType::END_OF_FILE, line} );
}

void Scanner::keywordMapInit()
{
    keywordMap["if"] = KeywordType::IF;
    keywordMap["int"] = KeywordType::INT;
}

bool Scanner::parsePunctuators(const char *c, unsigned int &index, unsigned int &line)
{
    Token token;
    token.type = TokenType::PUNCTUATOR;
    token.line = line;
    switch (c[index])
    {
    case '*':
        token.pType = PunctuatorType::STAR;
        tokenStream.push_back(token);
        return true;
    case '+':
        token.pType = PunctuatorType::PLUS;
        tokenStream.push_back(token);
        return true;
    case '-':
        token.pType = PunctuatorType::MINUS;
        tokenStream.push_back(token);
        return true;
    case '\\':
        token.pType = PunctuatorType::SLASH;
        tokenStream.push_back(token);
        return true;
    }
    return false;
}

bool Scanner::parseIdentifier(const char* c, unsigned int& index, unsigned int& line)
{
    if(isDigit(c[index]))
    {
        return false;
    }

    string currentString;
    while (isAlphaDigitFloor(c[index]) )
    {
        currentString += c[index];
        index++;
    }
    
    unordered_map<std::string, KeywordType>::iterator iter = keywordMap.find(currentString);
    if(iter != keywordMap.end())
    {
        Token token;
        token.type = TokenType::KEYWORD;
        token.kType = iter->second;
        token.line = line;
        tokenStream.push_back(token);
        return true;
    }

    Token token;
    token.type = TokenType::IDENTIFIER;
    token.line = line;
    token.data = new string(currentString);
    return true;
    
}

bool Scanner::isDigit(const char& c)
{
    return '0' <= c  && c <= '9';
}
bool Scanner::isAlpha(const char &c)
{
    return 'A' <= c  && c <= 'z';
}
bool Scanner::isAlphaDigitFloor(const char &c)
{
    return isDigit(c) || isAlpha(c) || c == '_';
}
Token Scanner::getCurrentToken()
{
    currentToken++;
    return tokenStream[currentToken - 1];
}