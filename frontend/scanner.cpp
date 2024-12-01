#include "scanner.hpp"
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
// keywords
    "BREAK", "CONST", "CONTINUE",
    "DO", "ELSE", "IF", "FOR",
    "RETURN", "WHILE",
//  keywords types
    "INT8", "INT16", "INT32", "INT64", 
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
    "COLON", "COMMA" , "SEMICOLON", "DOT", "INCREMENT", "DECREMENT",
};



Scanner::Scanner(const char *sourceCode)
    :
line(0), index(0)
{
    keywordMapInit();
}

void Scanner::keywordMapInit()
{
    keywordMap["int8"] = TokenType::INT8;
    keywordMap["int16"] = TokenType::INT16;
    keywordMap["int32"] = TokenType::INT32;
    keywordMap["int64"] = TokenType::INT64;
}

Token Scanner::parsePunctuators(const char *c)
{
    Token token;
    token.line = line;
    token.type = TokenType::NONE;
    switch (c[index])
    {
    case '*':
        index++;
        switch (c[index])
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
        index++;
        switch (c[index])
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
        index++;
        switch (c[index])
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
        index++;
        token.type = TokenType::L_BRACE;
        break;
    case '}':
        index++;
        token.type = TokenType::R_BRACE;
        break;
    case '(':
        index++;
        token.type = TokenType::L_PARENTHESES;
        break;
    case ')':
        index++;
        token.type = TokenType::R_PARENTHESES;
        break;
    case '/':
        index++;
        switch (c[index])
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
        index++;
        token.type = TokenType::SEMICOLON;
        break;
    case ',':
        index++;
        token.type = TokenType::COMMA;
        break;
    case '%':
        index++;
        switch (c[index])
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
        index++;
        switch (c[index])
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
        index++;
        switch (c[index])
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
        index++;
        token.type = TokenType::QUESTION_MARK;
        break;
    case '|':
        index++;
        switch (c[index])
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
        index++;
        switch (c[index])
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
        index++;
        token.type = TokenType::COLON;
        break;
    case '<':
        index++;
        if(c[index] == '=' )
        {
            token.type = TokenType::LESS_EQUAL;
            index++;
        }
        else if( c[index] == '<' && c[index + 1] == '=' )
        {
            token.type = TokenType::L_SHIFT_EQUAL;
            index +=2;
        }
        else if( c[index] == '<' )
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
        index++;
        if(c[index] == '=' )
        {
            token.type = TokenType::GREATER_EQUAL;
            index++;
        }
        else if( c[index] == '>' && c[index + 1] == '=' )
        {
            token.type = TokenType::R_SHIFT_EQUAL;
            index +=2;
        }
        else if( c[index] == '>' )
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

    return token;
}

Token Scanner::parseIdentifier(const char* c)
{
    if(isDigit(c[index]))
    {
        return Token{TokenType::NONE};
    }

    string* str = new string();
    string& currentString = *str;

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
        delete str;

        return token;
    }

    Token token;
    token.type = TokenType::IDENTIFIER;
    token.line = line;
    token.data = str;
    return token;
    
}

Token Scanner::parseConstant(const char *c)
{
    if( ! ('0' <= c[index] && c[index] <='9'))
    {
        return Token{TokenType::NONE};
    }


    if(c[index] == '0' &&  ('0' <= c[index + 1] && c[index + 1] <='9'))
    {
        fprintf(stdout, "incorrect integer definition at line %d\n", line);
        exit(-1);
    }


    string* str = new string();
    while (isDigit(c[index]))
    {
        (*str) += c[index];
        index++;
    }
    

    Token token;
    token.type = TokenType::CONSTANT;
    token.line = line;
    token.data = str;
    return token;
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
    Token* token;
    if( token_queue.size() == 0)
    {
        token_queue.push( getToken() );
    }
    token = &token_queue.front();

    auto iter = find(types.begin(), types.end(), token->type);
    return iter != types.end();
}

Token Scanner::getToken()
{
    if(src_buffer[index] == '\0')
    {
        Token token;
        token.type = TokenType::END_OF_FILE;
        token.line = line;
        return token;
    }

    while(src_buffer[index] == ' ' ||  src_buffer[index] == '\t' ||  src_buffer[index] == '\r' || src_buffer[index] == '\n')
    {
        if( src_buffer[index] == '\n')
        {
            line++;
        }
        index++;
    }
    Token token = parsePunctuators(src_buffer);
    if(token.type != TokenType::NONE)
    {
        return token;
    }

    token =parseIdentifier(src_buffer);
    if(token.type != TokenType::NONE)
    {
        return token;
    }

    token =parseIdentifier(src_buffer);
    if(token.type != TokenType::NONE)
    {
        return token;
    }
    
    printf("Something went wrong in scanner\n");
    exit(-1);
}

Token Scanner::peekToken()
{
    if(token_queue.size() != 0 )
    {
        return token_queue.front();
    }
    Token token = getToken();
    token_queue.push(token);
    return token;
}

bool Scanner::match(TokenType type)
{
    if(token_queue.size() == 0)
    {
        token_queue.push(getToken());
    }

    Token token = token_queue.front();
    if(token.type == type)
    {
        token_queue.pop();
        return true;
    }
    return false;
}
void Scanner::consume(TokenType type)
{
    Token token;
    if(token_queue.size() != 0)
    {
        token = token_queue.front();
        token_queue.pop();
    }
    else
    {
        token = getToken();
    }
    
    if(token.type == type)
    {
        return;
    }

    fprintf(stdout, "ERROR line %d: Recieved token is %s but expected is %s\n", 
            token.line + 1, TokenTypeString[(int)token.type ], TokenTypeString[(int)type]);
    exit(-1);
 }