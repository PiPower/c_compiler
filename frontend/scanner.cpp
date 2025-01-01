#include "scanner.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
using namespace std;

const char* tokenTypeString[] = {
    "NONE",
    "END_OF_FILE",
    "IDENTIFIER",
    "CONSTANT",
// keywords
    "BREAK", "CASE", "CONTINUE", "DO", "ELSE", "IF", "FOR",
    "RETURN", "WHILE", "TYPEDEF", "EXTERN", "STATIC",
    "AUTO", "REGISTER", "ENUM",
// type keywords
    "UNSIGNED", "INT", "LONG", "FLOAT", "DOUBLE", "IMAGINARY",
    "SIGNED", "CHAR", "SHORT", "VOID", "BOOL", "COMPLEX",
    // ----------------------
    "STRUCT", "UNION", "SIZEOF", "DEFAULT",
    "SWITCH", "INLINE",
 // type qualifier keywords
    "CONST", "VOLATILE", "RESTRICT",
// separators
    "L_BRACKET", "R_BRACKET", "L_PARENTHESES", "R_PARENTHESES",
    "L_BRACE", "R_BRACE",
 // math ops
    "AMPERSAND", "STAR", "PLUS", "MINUS", "TILDE", "BANG", "SLASH",
    "PERCENT", "LESS", "GREATER", "L_SHIFT",
    "R_SHIFT", "LESS_EQUAL", "GREATER_EQUAL", "EQUAL_EQUAL", "BANG_EQUAL",
    "PIPE", "CARET", "DOUBLE_AMPERSAND", "DOUBLE_PIPE", "QUESTION_MARK",
    "PLUS_PLUS", "MINUS_MINUS",
// assignment types
    "EQUAL", "STAR_EQUAL", "SLASH_EQUAL", "PERCENT_EQUAL", "PLUS_EQUAL",
    "MINUS_EQUAL", "L_SHIFT_EQUAL", "R_SHIFT_EQUAL", "AMPRESAND_EQUAL",
    "CARET_EQUAL", "PIPE_EQUAL",
 // miscellaneous
    "COLON", "COMMA", "SEMICOLON", "DOT", "ARROW",
 // for internal usage
    "COMMENT"
};

Scanner::Scanner(const char *sourceCode)
    :
line(1), index(0), src_buffer(sourceCode)
{
    keywordMapInit();
}

void Scanner::keywordMapInit()
{
    keywordMap["if"] = TokenType::IF;
    keywordMap["else"] = TokenType::ELSE;
    keywordMap["for"] = TokenType::FOR;
    keywordMap["while"] = TokenType::WHILE;
    keywordMap["do"] = TokenType::DO;
    keywordMap["return"] = TokenType::RETURN;
    keywordMap["continue"] = TokenType::CONTINUE;
    keywordMap["break"] = TokenType::BREAK;
    keywordMap["unsigned"] = TokenType::UNSIGNED;
    keywordMap["int"] = TokenType::INT;
    keywordMap["long"] = TokenType::LONG;
    keywordMap["float"] = TokenType::FLOAT;
    keywordMap["double"] = TokenType::DOUBLE;
    keywordMap["signed"] = TokenType::SIGNED;
    keywordMap["char"] = TokenType::CHAR;
    keywordMap["short"] = TokenType::SHORT;
    keywordMap["_Bool"] = TokenType::BOOL;
    keywordMap["_Complex"] = TokenType::COMPLEX;
    keywordMap["void"] = TokenType::VOID;
    keywordMap["typedef"] = TokenType::TYPEDEF;
    keywordMap["extern"] = TokenType::EXTERN;
    keywordMap["static"] = TokenType::STATIC;
    keywordMap["auto"] = TokenType::AUTO;
    keywordMap["struct"] = TokenType::STRUCT;
    keywordMap["union"] = TokenType::UNION;
    keywordMap["register"] = TokenType::REGISTER;
    keywordMap["const"] = TokenType::CONST;
    keywordMap["restrict"] = TokenType::RESTRICT;
    keywordMap["volatile"] = TokenType::VOLATILE;
    keywordMap["case"] = TokenType::CASE;
    keywordMap["default"] = TokenType::DEFAULT;
    keywordMap["switch"] = TokenType::SWITCH;
    keywordMap["inline"] = TokenType::INLINE;
    keywordMap["_Imaginary"] = TokenType::IMAGINARY;
}

void Scanner::skipWhitespace()
{
    while(src_buffer[index] == ' ' ||  src_buffer[index] == '\t' ||  
            src_buffer[index] == '\r' || src_buffer[index] == '\n')
    {
        if( src_buffer[index] == '\n')
        {
            line++;
        }
        index++;
    }
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
        case '+':
            token.type = TokenType::PLUS_PLUS;
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
        case '-':
            token.type = TokenType::MINUS_MINUS;
            index++;
            break;
        case '>':
            token.type = TokenType::ARROW;
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
    case '~':
        index++;
        token.type = TokenType::TILDE;
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
        case '/':
            token.type = TokenType::COMMENT;
            index++;
            while (c[index] != '\n' && c[index] != '\0'){index++;}
            if(c[index] == '\n' ){line++; index++;}
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
            token.type = TokenType::DOUBLE_AMPERSAND;
            index++;
            break;
        case '=':
            token.type = TokenType::AMPRESAND_EQUAL;
            index++;
            break;
        default:
            token.type = TokenType::AMPERSAND;
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
    case '.':
        index++;
        token.type = TokenType::DOT;
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
        token.data = str;

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
    if( !('0' <= c[index] && c[index] <='9') &&
        ((c[index] == '+' ||  c[index] == '-') && !('0' <= c[index + 1] && c[index + 1] <='9')) )
    {
        return Token{TokenType::NONE};
    }


    string* str = new string();
    while (isDigit(c[index]))
    {
        (*str) += c[index];
        index++;
    }
    if(c[index] == '.')
    {
        (*str) += c[index++];
        while (isDigit(c[index]))
        {
            (*str) += c[index];
            index++;
        }
        if( c[index] == 'f' || c[index] == 'F' ||
            c[index] == 'l' || c[index] == 'L')
        {
            (*str) += tolower(c[index++]);
        }
    }
    // check for int suffixes
    else if( c[index] == 'l' || c[index] == 'L' ||
            c[index] == 'u' || c[index] == 'U')
    {
        char suffix[4] = {0,0,0,0};

        int i = 0;
        while (i < 3 &&
                (c[index] == 'l' || c[index] == 'L' ||
                c[index] == 'u' || c[index] == 'U') )
        {
            suffix[i++] = tolower(c[index++]);
        }

        if((i == 3 && suffix[1] == 'u') || (i == 2 && suffix[0] == 'u' && suffix[1] == 'u'))
        {
            fprintf(stdout, "incorrect integer fromat at line %d\n", line);
            exit(-1);
        }
        *str += suffix;

    }

    if( isAlphaDigitFloor(c[index]))
    {
        fprintf(stdout, "incorrect integer definition at line %d\n", line);
        exit(-1);
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

bool Scanner::currentTokenOneOf(const std::vector<TokenType>& types)
{
    Token* token;
    if( token_queue.size() == 0)
    {
        token_queue.push_back( getToken() );
    }
    token = &token_queue.front();

    auto iter = find(types.begin(), types.end(), token->type);
    return iter != types.end();
}

bool Scanner::currentTokenOneOf(const TokenType *types, uint32_t tokenCount)
{
    Token* token;
    if( token_queue.size() == 0)
    {
        token_queue.push_back( getToken() );
    }
    token = &token_queue.front();

    for (uint32_t tokenIdx = 0; tokenIdx < tokenCount; tokenIdx++)
    {
        if(types[tokenIdx] == token->type)
        {
            return true;
        }
    }
    

    return false;
}

void Scanner::putfront(Token token)
{
    token_queue.push_front(token);
}

Token Scanner::getToken()
{
    if(token_queue.size() != 0)
    {
        Token token = token_queue.front();
        token_queue.pop_front();
        return token;
    }
 
 parseToken:
    skipWhitespace();

    if(src_buffer[index] == '\0')
    {
        Token token;
        token.type = TokenType::END_OF_FILE;
        token.line = line;
        return token;
    }

    Token token = parsePunctuators(src_buffer);
    if( token.type == TokenType::COMMENT)
    {
        goto parseToken;
    }
    if(token.type != TokenType::NONE)
    {
        return token;
    }

    token = parseIdentifier(src_buffer);
    if(token.type != TokenType::NONE)
    {
        return token;
    }

    token = parseConstant(src_buffer);
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
    token_queue.push_back(token);
    return token;
}

Token Scanner::peekNextToken()
{
    while (token_queue.size() < 2 )
    {
        token_queue.push_back(getToken());
    }
    Token holder = token_queue.front();
    token_queue.pop_front();
    Token targetToken = token_queue.front();
    token_queue.push_front(holder);
    return targetToken;
}

bool Scanner::match(TokenType type)
{
    if(token_queue.size() == 0)
    {
        token_queue.push_back(getToken());
    }

    Token token = token_queue.front();
    if(token.type == type)
    {
        token_queue.pop_front();
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
        token_queue.pop_front();
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
            token.line, tokenTypeString[(int)token.type], tokenTypeString[(int)type]);
    exit(-1);
 }