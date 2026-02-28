#include "Lexer.hpp"
#include <cassert>
#include <stdio.h>
#include <string.h>
Lexer::Lexer(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
mainFile(mainFile), manager(manager), currChar(0), opts(opts)
{
    assert(manager != nullptr);
    assert(opts != nullptr);
    charHistory.reserve(100000);
    FilePos initialFile;
    manager->GetFileId(mainFile.path, mainFile.pathLen, &initialFile.fileId);
    initialFile.lineNr = 1;
    initialFile.fileBase = mainFile.fileData;
    initialFile.fileEnd = mainFile.fileData + mainFile.fileSize;
    files.push(initialFile);

    fCurr = initialFile.fileBase;
    fEnd = initialFile.fileEnd;
}

bool Lexer::IsHorizontalWhiteSpace(char C)
{
    return C == ' ' || C == '\v' || C == '\f' || C == '\t'; 
}

bool Lexer::IsVerticalWhiteSpace(char C)
{
    return C == '\n' || C == '\r';
}

bool Lexer::IsWhiteSpace(char C)
{
    return IsVerticalWhiteSpace(C) || IsHorizontalWhiteSpace(C);
}

bool Lexer::IsSimpleChar(char C)
{
    return C != '?' && C != '\\';
}

char Lexer::GetCurrChar()
{
    if(currChar < charHistory.size())
    {
        return charHistory[currChar];
    }
    return GetNextChar();
}

char Lexer::GetNextChar()
{
    if(fCurr == fEnd)
    {
        if(charHistory.back() != '\0') 
        {
            charLocations.emplace(files.top(), fCurr, 1);
            charHistory.push_back('\0');
        }
        return charHistory.back();
    }

    if(IsSimpleChar(*fCurr))
    {
        charHistory.push_back(*fCurr);
        charLocations.emplace(files.top(), fCurr, 1);
        fCurr++;
        return charHistory.back();
    }

    const char* fCurrBuff = fCurr;
    charHistory.push_back( GetCharSlow());
    charLocations.emplace(files.top(), fCurrBuff, fCurr - fCurrBuff);
    return charHistory.back();
}

char Lexer::GetCharSlow()
{
    uint64_t remainingChars = fEnd - fCurr;
    if(*fCurr == '\\')
    {
slash:
        printf("Lexer internal: Possibly incorrect execution path \n");
        fflush(stdout);
        if(remainingChars > 0 && !IsWhiteSpace(*(fCurr + 1)))
        {
            remainingChars--;
            fCurr++;
            return '\\';
        }
        //TODO add support for escaped-newline
        fCurr++;
        return '\\';
    }
    
    // process trigraph
    if(opts->trigraphs_enabled && remainingChars >= 2 && fCurr[0] == '?' && fCurr[1] == '?')
    {
        char out = '\0';
        switch (fCurr[2])
        {
        case '/':
            remainingChars -= 3;
            fCurr += 3;
            goto slash;
            break;
        case '=':  out = '#'; break;
        case '\'': out = '^'; break;
        case '(':  out = '['; break; 
        case ')':  out = ']'; break;    
        case '!':  out = '|'; break;
        case '<':  out = '{'; break;
        case '>':  out = '}'; break;    
        case '-':  out = '~'; break;  
        default: return *fCurr;
        }
        if(opts->trigraphs_refrenced == 0)
        {
            FILE_STATE fileState;
            manager->GetFileState(&files.top().fileId, &fileState);
            char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
            memcpy(pathBuffer, fileState.path, fileState.pathLen);
            pathBuffer[fileState.pathLen] = '\0';
            
            printf("%s:%ld WARNING: Trigraph detected in source code, in order to hide warning use flag -ftrigraphs\n", 
                    pathBuffer, files.top().lineNr);
        }
        fCurr += 3;
        return out;
    }
    return *fCurr;
}

char Lexer::LookAhead(size_t n)
{
    if(currChar + n < charHistory.size())
    {
        return charHistory[currChar + n];
    }
    size_t currCharBuffer = currChar;
    char C;
    while (n > 0)
    {
        C = GetNextChar();
        currChar++;
        n--;
    }
    
    currChar = currCharBuffer;
    return C;
}

void Lexer::ConsumeChar()
{
    currChar++;
    charLocations.pop();
    assert(currChar<=charHistory.size());
}

void Lexer::SkipHorizonthalWhiteSpace()
{
    while (fCurr < fEnd && IsHorizontalWhiteSpace(*fCurr))
    {
        fCurr++;
    }
}

int32_t Lexer::Lex(Token* token)
{
    SkipHorizonthalWhiteSpace();

    char C = GetCurrChar();
    ConsumeChar();
    switch (C)
    {
    // parsing punctuators
    case '*':
        C = GetCurrChar();
        if(C == '=')
        {
            token->type = TokenType::star_equal;
            ConsumeChar();
        }
        else{token->type = TokenType::star;}
        break;
    case '+':
        C = GetCurrChar();
        if(C == '=')
        {
            token->type = TokenType::plus_equal;
            ConsumeChar();
        }
        if(C == '+')
        {
            token->type = TokenType::plus_plus;
            ConsumeChar();
        }
        else {token->type = TokenType::plus;}
        break;
    case '-': 
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::minus_equal;
            ConsumeChar();
        }
        else if (C == '-')
        {
            token->type = TokenType::minus_minus;
            ConsumeChar();
        }
        else if (C == '>')
        {
            token->type = TokenType::arrow;
            ConsumeChar();
        }
        else{ token->type = TokenType::minus;}
        break;
    case '/':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::slash_equal;
            ConsumeChar();
        }
        else if (C == '/')
        {
            token->type = TokenType::comment;
            ConsumeChar();
            C = GetCurrChar();
            while (C != '\n' && C != '\0') {C = GetCurrChar(); ConsumeChar(); }
            if (C == '\n') { files.top().lineNr++; ConsumeChar(); }
        }
        else{token->type = TokenType::slash;}
        break;
    case '%':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::percent_equal;
            ConsumeChar();
        }
        else{token->type = TokenType::percent;}
        break;
    case '=':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::equal_equal;
            ConsumeChar();
        }
        else{token->type = TokenType::equal;}
        break;
    case '^':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::caret_equal;
            ConsumeChar();
        }
        else{token->type = TokenType::caret;}
        break;
    case '|':
        C = GetCurrChar();
        if (C == '|')
        {
            token->type = TokenType::double_pipe;
            ConsumeChar();
        }
        else if (C == '=')
        {
            token->type = TokenType::pipe_equal;
            ConsumeChar();
        }
        else{token->type = TokenType::pipe;}
        break;
    case '!':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::bang_equal;
            ConsumeChar();
        }
        else{token->type = TokenType::bang;}
        break;
    case '&':
        C = GetCurrChar();
        if (C == '&')
        {
            token->type = TokenType::double_ampersand;
            ConsumeChar();
        }
        else if (C == '=')
        {
            token->type = TokenType::ampresand_equal;
            ConsumeChar();
        }
        else{token->type = TokenType::ampersand;}
        break;
    case '<':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::less_equal;
            ConsumeChar();
        }
        else if (C == '<')
        {
            ConsumeChar();
            C = GetCurrChar();
            if(C == '=') 
            {
                token->type = TokenType::l_shift_equal;
                ConsumeChar();
            }
            else{token->type = TokenType::l_shift;}
        }
        else{token->type = TokenType::less;}
        break;
    case '>':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::greater_equal;
            ConsumeChar();
        }
        else if (C == '>')
        {
            ConsumeChar();
            C = GetCurrChar();
            if(C == '=') 
            {
                token->type = TokenType::r_shift_equal;
                ConsumeChar();
            }
            else{token->type = TokenType::r_shift;}
        }
        else{token->type = TokenType::greater;}
        break;
    case '.': 
        C = GetCurrChar();
        if(C == '.' && LookAhead(1) == '.')
        {
            token->type = TokenType::ellipsis; 
            ConsumeChar();
            ConsumeChar();
        }
        else{token->type = TokenType::dot; }
        break;
    case '{': token->type = TokenType::l_brace; break;
    case '~': token->type = TokenType::tilde; break;
    case '}': token->type = TokenType::r_brace; break;
    case '(': token->type = TokenType::l_parentheses; break;
    case ')': token->type = TokenType::r_parentheses; break;
    case ';': token->type = TokenType::semicolon; break;
    case ',': token->type = TokenType::comma; break;
    case '?': token->type = TokenType::question_mark; break;
    case ':': token->type = TokenType::colon; break;
    case '\n': token->type = TokenType::colon; files.top().lineNr++; break;

    // parsing punctuators
    default:
        break;
    }

    return 0;
}
