#include "../../include/frontend/scanner.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

const char* TokenTypeString[]
{
    "NONE",
    "END_OF_FILE",
    "IDENTIFIER",
    "CONSTANT",
    "STRING_LITERAL",
// keywords
    "AUTO", "BREAK", "CASE", "CONST", "CONTINUE",
    "DEFAULT", "DO", "ELSE", "ENUM", "EXTERN", "FOR",
    "GOTO", "IF", "INLINE", "REGISTER", "RESTRICT",
    "RETURN", "SIZEOF", "STATIC", "STRUCT", "SWITCH",
    "TYPEDEF", "UNION", "VOLATILE", "WHILE",
//  keywords types
    "VOID", "CHAR", "SHORT", "INT", "LONG", "FLOAT", 
    "DOUBLE", "SIGNED", "UNSIGNED", "_BOOL", "_COMPLEX",
// separators
    "L_BRACKET",  "R_BRACKET", "L_PARENTHESES",  "R_PARENTHESES", 
    "L_BRACE", "R_BRACE",
// math ops  
    "AMPRESAND", "STAR", "PLUS", "MINUS", "TILDE", "BANG", "SLASH", 
    "PERCENT", "LESS", "GREATER",  "L_SHIFT",
    "R_SHIFT", "LESS_EQUAL", "GREATER_EQUAL", "EQUAL_EQUAL", "BANG_EQUAL",
    "PIPE", "CARET", "DOUBLE_AMPRESAND", "DOUBLE_PIPE", "QUESTION_MARK",
// assignemnt types
    "EQUAL", "STAR_EQUAL", "SLASH_EQUAL", "PERCENT_EQUAL", "PLUS_EQUAL", 
    "MINUS_EQUAL", "L_SHIFT_EQUAL", "R_SHIFT_EQUAL", "AMPRESAND_EQUAL",
    "CARET_EQUAL", "PIPE_EQUAL",
// miscallenous
     "COLON", "COMMA" , "SEMICOLON", "DOT", "ARROW", "INCREMENT", "DECREMENT",
};



Scanner::Scanner(const char *sourceCode)
:
currentToken(0)
{
    keywordMapInit();

    unsigned int i =0;
    unsigned int line = 0;
    while (sourceCode[i] != '\0')
    {
        if(sourceCode[i] == ' ' ||  sourceCode[i] == '\t' ||  sourceCode[i] == '\r' || sourceCode[i] == '\n')
        {
            if( sourceCode[i] == '\n')
            {
                line++;
            }
            i++;
            continue;
        }
        if(parsePunctuators(sourceCode, i, line))
        {
            i++;
            continue;
        }
        if(parseIdentifier(sourceCode, i, line))
        {
            continue;
        }
        if(parseConstant(sourceCode, i, line))
        {
            continue;
        }
        i++;
    }
    
    tokenStream.push_back( Token{TokenType::END_OF_FILE, line} );
}

void Scanner::keywordMapInit()
{
    keywordMap["if"] = TokenType::IF;
    keywordMap["else"] = TokenType::ELSE;
    keywordMap["int"] = TokenType::INT;
    keywordMap["while"] = TokenType::WHILE;
    keywordMap["for"] = TokenType::FOR;
    keywordMap["do"] = TokenType::DO;
    keywordMap["typedef"] = TokenType::TYPEDEF;
    keywordMap["extern"] = TokenType::EXTERN;
    keywordMap["static"] = TokenType::STATIC;
    keywordMap["auto"] = TokenType::AUTO;
    keywordMap["register"] = TokenType::REGISTER;
    keywordMap["inline"] = TokenType::INLINE;
}

bool Scanner::parsePunctuators(const char *c, unsigned int &index, unsigned int &line)
{
    Token token;
    token.line = line;
    token.type = TokenType::NONE;
    switch (c[index])
    {
    case '*':
        switch (c[index + 1])
        {
        case '=':
            token.type = TokenType::STAR_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::STAR;
            break;
        }
        break;
    case '+':
        switch (c[index + 1])
        {
        case '=':
            token.type = TokenType::PLUS_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::PLUS;
            break;
        }
        break;
    case '-':
        switch (c[index + 1])
        {
        case '=':
            token.type = TokenType::MINUS_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::MINUS;
            break;
        }
        break;
    case '{':
        token.type = TokenType::L_BRACE;
        break;
    case '}':
        token.type = TokenType::R_BRACE;
        break;
    case '(':
        token.type = TokenType::L_PARENTHESES;
        break;
    case ')':
        token.type = TokenType::R_PARENTHESES;
        break;
    case '/':
        switch (c[index + 1])
        {
        case '=':
            token.type = TokenType::SLASH_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::SLASH;
            break;
        }
        break;
    case ';':
        token.type = TokenType::SEMICOLON;
        break;
    case ',':
        token.type = TokenType::COMMA;
        break;
    case '%':
        switch (c[index + 1])
        {
        case '=':
            token.type = TokenType::PERCENT_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::PERCENT;
            break;
        }
        break;
    case '=':
        switch (c[index + 1])
        {
        case '=':
            token.type = TokenType::EQUAL_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::EQUAL;
            break;
        }
        break;
    case '^':
        switch (c[index + 1])
        {
        case '=':
            token.type = TokenType::CARET_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::CARET;
            break;
        }
        break;

    case '?':
        token.type = TokenType::QUESTION_MARK;
        break;
    case '|':
         switch (c[index + 1])
        {
        case '|':
            token.type = TokenType::DOUBLE_PIPE;
            index++;
            break;
        case '=':
            token.type = TokenType::PIPE_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::PIPE;
            break;
        }
        break;
    case '&':
         switch (c[index + 1])
        {
        case '&':
            token.type = TokenType::DOUBLE_AMPRESAND;
            index++;
            break;
        case '=':
            token.type = TokenType::AMPRESAND_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::AMPRESAND;
            break;
        }
        break;
    case ':':
        token.type = TokenType::COLON;
        break;
    case '<':
        if(c[index + 1] == '=' )
        {
            token.type = TokenType::LESS_EQUAL;
            index++;
        }
        else if( c[index + 1] == '<' && c[index + 2] == '=' )
        {
            token.type = TokenType::L_SHIFT_EQUAL;
            index +=2;
        }
        else if( c[index + 1] == '<' )
        {
            token.type = TokenType::L_SHIFT;
            index++;
        }
        else
        {
            token.type = TokenType::LESS;
        }
        break;
    case '>':
        if(c[index + 1] == '=' )
        {
            token.type = TokenType::GREATER_EQUAL;
            index++;
        }
        else if( c[index + 1] == '>' && c[index + 2] == '=' )
        {
            token.type = TokenType::R_SHIFT_EQUAL;
            index +=2;
        }
        else if( c[index + 1] == '>' )
        {
            token.type = TokenType::R_SHIFT;
            index++;
        }
        else
        {
            token.type = TokenType::GREATER;
        }
        break;
    }
    if( token.type != TokenType::NONE)
    {
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
    
    unordered_map<std::string, TokenType>::iterator iter = keywordMap.find(currentString);
    if(iter != keywordMap.end())
    {
        Token token;
        token.type = iter->second;
        token.line = line;
        tokenStream.push_back(token);
        return true;
    }

    Token token;
    token.type = TokenType::IDENTIFIER;
    token.line = line;
    token.context.data = new string(currentString);
    tokenStream.push_back(token);
    return true;
    
}

bool Scanner::parseConstant(const char *c, unsigned int &index, unsigned int &line)
{
    if( ! ('0' <= c[index] && c[index] <='9'))
    {
        return false;
    }


    if(c[index] == '0' &&  ('0' <= c[index + 1] && c[index + 1] <='9'))
    {
        fprintf(stdout, "incorrect integer definition at line %d\n", line);
        exit(-1);
    }


    int value = 0;
    while (isDigit(c[index]))
    {
        value *= 10;
        value += (int)c[index] - (int)'0';
        index++;
    }
    

    Token token;
    token.type = TokenType::CONSTANT;
    token.line = line;
    token.context.int_32 = value;
    tokenStream.push_back(token);
    return true;
}
bool Scanner::isDigit(const char &c)
{
    return '0' <= c  && c <= '9';
}

bool Scanner::isAlpha(const char &c)
{
    return ('A' <= c  && c <= 'Z') || ('a' <= c  && c <= 'z' );
}

bool Scanner::isAlphaDigitFloor(const char &c)
{
    return isDigit(c) || isAlpha(c) || c == '_';
}


bool Scanner::currentTokenOneOf(std::vector<TokenType> types)
{
    auto iter = find(types.begin(), types.end(), tokenStream[currentToken].type);
    return iter != types.end();
}

Token Scanner::getCurrentToken()
{
    return tokenStream[currentToken];
}

void Scanner::incrementTokenId()
{
    currentToken++;
}

Token Scanner::popToken()
{
    currentToken++;
    return tokenStream[currentToken - 1];

}
Token Scanner::peekToken()
{
    return tokenStream[currentToken + 1];
}

bool Scanner::match(TokenType type)
{
    if(tokenStream[currentToken].type == type)
    {
        currentToken++;
        return true;
    }
    return false;
}
void Scanner::consume(TokenType type)
{
    if(tokenStream[currentToken].type == type)
    {
        currentToken++;
        return;
    }

    fprintf(stdout, "ERROR line %d: Recieved token is %s but expected is %s\n", 
            tokenStream[currentToken].line + 1, TokenTypeString[(int)tokenStream[currentToken].type ],TokenTypeString[(int)type]);
    exit(-1);
 }