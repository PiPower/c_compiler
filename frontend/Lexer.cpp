#include "Lexer.hpp" 
#include <cassert>
#include <stdio.h>
#include <string.h>
Lexer::Lexer(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
mainFile(mainFile), manager(manager), opts(opts)
{
    assert(manager != nullptr);
    assert(opts != nullptr);

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

void Lexer::TrigraphWarning(SourceLocation loc)
{
    TrigraphWarning(&loc);
}

void Lexer::TrigraphWarning(const SourceLocation* loc)
{
    if(opts->trigraphs_refrenced == 0)
    {
        FILE_STATE fileState;
        manager->GetFileState(&files.top().fileId, &fileState);
        char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
        memcpy(pathBuffer, fileState.path, fileState.pathLen);
        pathBuffer[fileState.pathLen] = '\0';
        
        printf("%s:%ld:%ld WARNING: Trigraph detected in source code, in order to hide warning use flag -ftrigraphs\n", 
                pathBuffer, loc->line, loc->offset);
    }
}

bool Lexer::IsWhiteSpace(char C)
{
    return IsVerticalWhiteSpace(C) || IsHorizontalWhiteSpace(C);
}

bool Lexer::IsSimpleChar(char C)
{
    return C != '?' && C != '\\';
}

void Lexer::ChangeLexedFile()
{
    // currently multiple files are not supported
    exit(-1);
}

SourceLocation Lexer::GetCurrLoc()
{
    return currLocations.front();
}

char Lexer::GetCurrChar()
{
    if(!charsQueue.empty())
    {
        return charsQueue.front();
    }
    return GetNextChar();
}

char Lexer::GetNextChar()
{
    if(fCurr == fEnd)
    {
        if(charsQueue.empty() || charsQueue.front() != '\0') 
        {
            currLocations.emplace(files.top(), fCurr, 1);
            charsQueue.emplace_back('\0');
        }
        return charsQueue.front();
    }

    if(IsSimpleChar(*fCurr))
    {
        charsQueue.emplace_back(*fCurr);
        currLocations.emplace(files.top(), fCurr, 1);
        fCurr++;
        return charsQueue.front();
    }

    const char* fCurrBuff = fCurr;
    charsQueue.emplace_back( GetCharSlow());
    currLocations.emplace(files.top(), fCurrBuff, fCurr - fCurrBuff);
    return charsQueue.front();
}

char Lexer::GetCharSlow()
{
    uint64_t remainingChars = fEnd - fCurr;
    if(*fCurr == '\\')
    {
slash:
        printf("Lexer internal: Possibly incorrect execution path \n");
        fflush(stdout);
        if(remainingChars > 1 && !IsWhiteSpace(*(fCurr + 1)))
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
    if(opts->trigraphs_enabled && remainingChars > 2 && fCurr[0] == '?' && fCurr[1] == '?')
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
        default:  fCurr++; return *(fCurr-1);
        }
        fCurr += 3;
        return out;
    }
    fCurr++;
    return *(fCurr-1);
}

void Lexer::LexIdentifier()
{
}

char Lexer::LookAhead(size_t n)
{
    if(n < charsQueue.size())
    {
        return charsQueue.at(n);
    }

    char C = '\0';
    while (n > 0)
    {
        C = GetNextChar();
        n--;
    }
    
    return C;
}

void Lexer::ConsumeChar()
{
    charsQueue.pop_front();
    currLocations.pop();
}

void Lexer::SkipHorizontalWhiteSpace()
{
    while (fCurr < fEnd && IsHorizontalWhiteSpace(*fCurr))
    {
        fCurr++;
    }
}

int64_t Lexer::LexComment()
{
    // in order to correctly parse comment 
    // it is needed to move fCurr to offset after second \ and skip line
    // additionally both charHistory and charLocations need to be invalidated
    SourceLocation loc = GetCurrLoc();
    int64_t commentLen = 0;

    charsQueue.clear();
    while (!currLocations.empty()) { currLocations.pop(); }
    
    fCurr = files.top().fileBase + loc.offset;
skip_loop:
    while (fCurr < files.top().fileEnd && *fCurr != '\n')
    {
        commentLen++;
        fCurr++;
    }

    if (fCurr < files.top().fileEnd && *fCurr == '\n')
    {
        files.top().lineNr++;
        fCurr++;
    }

    if(fCurr == files.top().fileEnd && files.size() > 0)
    {
        ChangeLexedFile();
        fCurr = files.top().fileCurrent;
        fEnd = files.top().fileEnd;
        goto skip_loop;
    }

    return commentLen;
}

int32_t Lexer::Lex(Token* token)
{
    token->type = TokenType::none;
    token->location = {};

    SkipHorizontalWhiteSpace();

    char C = GetCurrChar();
    token->location = GetCurrLoc();
    LookAhead(7);
    ConsumeChar();

    switch(C)
    {
    // parsing punctuators
    case '*':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::star_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else{token->type = TokenType::star;}
        break;
    case '+':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::plus_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '+')
        {
            token->type = TokenType::plus_plus;
            token->location.len = 2;
            ConsumeChar();
        }
        else{token->type = TokenType::plus;}
        break;
    case '-':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::minus_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '-')
        {
            token->type = TokenType::minus_minus;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '>')
        {
            token->type = TokenType::arrow;
            token->location.len = 2;
            ConsumeChar();
        }
        else{token->type = TokenType::minus;}
        break;
    case '/':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::slash_equal;
            token->location.len = 2;
            ConsumeChar();
        } 
        else if (C == '/')
        {
            token->type = TokenType::comment;
            token->location.len = 2; // "//"
            ConsumeChar();
            token->location.len += LexComment();
        }
        else{token->type = TokenType::slash;}
        break;
    case '%':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::percent_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '>')
        {
            token->type = TokenType::r_brace;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == ':')
        {
            token->type = TokenType::hash;
            token->location.len = 2;
            ConsumeChar();
            C = GetCurrChar();
            char C1 = LookAhead(1);
            if(C == '%' && C1 == ':')
            {
                token->type = TokenType::d_hash;
                token->location.len = 4;
                ConsumeChar();
                ConsumeChar();
            }

        }
        else{token->type = TokenType::percent;}break;
    case '=':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::equal_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else{token->type = TokenType::equal;}
        break;
    case '^':
        if(token->location.len != 1){TrigraphWarning(&token->location);} 
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::caret_equal;
            token->location.len += 1;
            ConsumeChar();
        }
        else{token->type = TokenType::caret;}
        break;
    case '|':
        if(token->location.len != 1){TrigraphWarning(&token->location);} 
        C = GetCurrChar();
        if (C == '|')
        {
            if(GetCurrLoc().len != 1){TrigraphWarning(GetCurrLoc());} 
            token->type = TokenType::double_pipe;
            token->location.len += GetCurrLoc().len;
            ConsumeChar();
        }
        else if (C == '=')
        {
            token->type = TokenType::pipe_equal;
            token->location.len += 1;
            ConsumeChar();
        }
        else{token->type = TokenType::pipe;}
        break;
    case '!':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::bang_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else{token->type = TokenType::bang;}
        break;
    case '&':
        C = GetCurrChar();
        if (C == '&')
        {
            token->type = TokenType::double_ampersand;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '=')
        {
            token->type = TokenType::ampresand_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else{token->type = TokenType::ampersand;}
        break;
    case '<':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::less_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == ':')
        {
            token->type = TokenType::l_bracket;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '%')
        {
            token->type = TokenType::l_brace;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '<')
        {
            ConsumeChar();
            C = GetCurrChar();
            if (C == '=')
            {
                token->type = TokenType::l_shift_equal;
                token->location.len = 3;
                ConsumeChar();
            }
            else
            {
                token->type = TokenType::l_shift;
                token->location.len = 2;
            }
        }
        else{token->type = TokenType::less;}
        break;
    case '>':
        C = GetCurrChar();
        if (C == '=')
        {
            token->type = TokenType::greater_equal;
            token->location.len = 2;
            ConsumeChar();
        }
        else if (C == '>')
        {
            ConsumeChar();
            C = GetCurrChar();
            if (C == '=')
            {
                token->type = TokenType::r_shift_equal;
                token->location.len = 3;
                ConsumeChar();
            }
            else
            {
                token->type = TokenType::r_shift;
                token->location.len = 2;
            }
        }
        else{token->type = TokenType::greater;}
        break;
    case '.':
        C = GetCurrChar();
        if (C == '.' && LookAhead(1) == '.')
        {
            token->type = TokenType::ellipsis;
            token->location.len = 3;
            ConsumeChar();
            ConsumeChar();
        }
        else{token->type = TokenType::dot;}
        break;
    case '#':
        if(token->location.len != 1){TrigraphWarning(&token->location);}
        C = GetCurrChar();
        if(C == '#')
        {
            if(GetCurrLoc().len != 1){TrigraphWarning(GetCurrLoc());}

            token->type = TokenType::d_hash;
            token->location.len += GetCurrLoc().len;
            ConsumeChar();
        }
        else {token->type = TokenType::hash;} 
        break;
    case ':': 
        C = GetCurrChar();
        if(C == '>')
        {
            token->type = TokenType::r_bracket;
            token->location.len = 2;
            ConsumeChar();
        }
        else {token->type = TokenType::colon;} 
        break;

    case '{': token->type = TokenType::l_brace;        if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '~': token->type = TokenType::tilde;          if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '}': token->type = TokenType::r_brace;        if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '[': token->type = TokenType::l_bracket;      if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case ']': token->type = TokenType::r_bracket;      if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '(': token->type = TokenType::l_parentheses;  break;
    case ')': token->type = TokenType::r_parentheses;  break;
    case ';': token->type = TokenType::semicolon;      break;
    case ',': token->type = TokenType::comma;          break;
    case '?': token->type = TokenType::question_mark;  break;
    case '\n':
        token->type = TokenType::new_line;
        token->location.len = 1;
        files.top().lineNr++;
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        exit(-1);
    default:
        LexIdentifier();
        break;
    }

    // remove remaining white space chars
    while (IsHorizontalWhiteSpace(GetCurrChar()))
    {
        ConsumeChar();
    }
    
    return 0;
}
