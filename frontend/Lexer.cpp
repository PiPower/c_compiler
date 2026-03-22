#include "Lexer.hpp" 
#include <cassert>
#include <stdio.h>
#include <string.h>

static constexpr int8_t dec_type_dec = 0;
static constexpr int8_t dec_type_oct = 1;
static constexpr int8_t dec_type_hex = 2;
static constexpr int8_t dec_type_bin = 3;

static const char* kewordStrings[] = {
    "auto",      "break",     "case",       "char",
    "const",     "continue",  "default",    "do",
    "double",    "else",      "enum",       "extern",
    "float",     "for",       "goto",       "if",
    "inline",    "int",       "long",       "register",
    "restrict",  "return",    "short",      "signed",
    "sizeof",    "static",    "struct",     "switch",
    "typedef",   "union",     "unsigned",   "void",
    "volatile",  "while",     "_Bool",      "_Complex",
    "_Imaginary",
    // preprocessor  specific keywords,
    "include",   "define",    "ifdef",  "ifndef",  
    "elif",      "endif",     "line",   "error", 
    "pragma",    "undef",     "defined"
};


Lexer::Lexer(FileManager* manager, const CompilationOpts* opts)
:
manager(manager), fEnd(nullptr), fCurr(nullptr), opts(opts)
{
    assert(manager != nullptr);
    assert(opts != nullptr);
    PrepareKeywordMap();
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
            SourceLocation loc = files.size() != 0 ?
                ConstructLocation(files.top(), fCurr, 1) :
                lastFileLoc;

            currLocations.push(loc);
            charsQueue.emplace_back('\0');
        }
        return charsQueue.front();
    }

    if(IsSimpleChar(*fCurr))
    {
        charsQueue.emplace_back(*fCurr);
        currLocations.push(ConstructLocation(files.top(), fCurr, 1));
        fCurr++;
        return charsQueue.front();
    }

    const char* fCurrBuff = fCurr;
    charsQueue.emplace_back( GetCharSlow());
    currLocations.push(ConstructLocation(files.top(), fCurrBuff, fCurr - fCurrBuff));
    return charsQueue.front();
}

char Lexer::GetCharSlow()
{
    uint64_t remainingChars = fEnd - fCurr;
    if(*fCurr == '\\')
    {
slash:
        //printf("Lexer internal warning: Possibly incorrect execution path \n");
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

void Lexer::LexIdentifier(Token* token, const SourceLocation* firstChar)
{
    RestoreLexerPointer();
    // set fCurr to first char in buffer
    fCurr = files.top().fileBase + firstChar->offset;
    size_t maxLen = fEnd - fCurr;
    if(!IsNonDigit(fCurr, maxLen))
    {
        token->PossiblyErronous = 1;
    }
    
    const char* identifierStart = fCurr;
    int64_t len = 0; 
    int64_t UCN  = IsUniversalChar(fCurr, maxLen);
    int64_t offset = UCN > 0 ? UCN : 1;
    fCurr += offset;
    len += offset;
    maxLen -= offset;

    while (fCurr < fEnd)
    {
        bool simpleChar = IsAlphaDigitFloor(*fCurr);
        int64_t UCN = IsUniversalChar(fCurr, maxLen);
        if(simpleChar)
        {
            fCurr++;
            len++;
            maxLen--;
        }
        else if(UCN > 0)
        {
            fCurr += UCN;
            len += UCN;
            maxLen -= UCN;
        }
        else
        {
            break;
        }
    }
 
    std::string_view key(identifierStart, len);
    auto hMapEntry = keywordsMap.find(key);
    if(hMapEntry != keywordsMap.end())
    {
        token->type = hMapEntry->second;
    }
    else
    {
        token->type = TokenType::identifier;
    }
    token->location.id = files.top().fileId;
    token->location.len = len;
    token->location.line = files.top().lineNr;
    token->location.offset = identifierStart - files.top().fileBase;
}

void Lexer::LexCharSequence(Token *token, const char separator)
{
    RestoreLexerPointer();
    bool error = false;
    const char* CharSeqStart = fCurr - 1;
 
    while (fCurr < fEnd && *fCurr != separator )
    {
        if( *fCurr != '\\') {fCurr++;}
        if((*fCurr == '\\' && !LexEscapeSequence() )|| *fCurr == '\n')
        {
            error = true;
            break;
        }
        
    }
    token->location = ConstructLocation(files.top(), CharSeqStart, fCurr - CharSeqStart);

    fCurr++;
    if(error)
    {
        FILE_STATE fileState;
        manager->GetFileState(&files.top().fileId, &fileState);
        char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
        memcpy(pathBuffer, fileState.path, fileState.pathLen);
        pathBuffer[fileState.pathLen] = '\0';
        int64_t offset = fCurr - files.top().fileBase;

        printf("%s:%ld:%ld ERROR: Incorrect char sequence\n", 
        pathBuffer, files.top().lineNr, offset);
    }
    return;
}

bool Lexer::LexEscapeSequence()
{
    RestoreLexerPointer();
    if(fCurr + 1 >= fEnd)
    {
        return false;
    }

    fCurr++;
    if(*fCurr == '\'' || *fCurr == '\"'  || *fCurr == '?' ||
        *fCurr == '\\' || *fCurr == 'a'  || *fCurr == 'b' ||
        *fCurr == 'f' || *fCurr == 'n'  || *fCurr == 'r' ||
        *fCurr == 't' || *fCurr == 'v')
    {
        fCurr++;
        return true;
    }
    else if(*fCurr == 'x' && fCurr + 1 < fEnd && IsHexDigit(*(fCurr + 1)))
    {
        fCurr+=2;
        while (fCurr < fEnd && IsHexDigit(*fCurr))
        {
            fCurr++;
        }
        return true;
    }
    else if(IsOctalDigit(*fCurr))
    {
        fCurr++;
        if(fCurr < fEnd && IsOctalDigit(*fCurr)) {fCurr++;}
        if(fCurr < fEnd && IsOctalDigit(*fCurr)) {fCurr++;}
        return true;
    }
    return false;
}

/*
    Sets fCurr to first char to be processed
*/
void Lexer::RestoreLexerPointer()
{
    assert(charsQueue.empty() && currLocations.empty());
    if(!charsQueue.empty())
    {
        SourceLocation loc = GetCurrLoc();
        if(loc.id.id != files.top().fileId.id)
        {
            printf("lexer pointer restoration is not working between files \n");
            exit(-1);
        }

        charsQueue.clear();
        while (!currLocations.empty()) { currLocations.pop(); }
        fCurr = files.top().fileBase + loc.offset;
    }
}

void Lexer::IssueWarning(const char *msg, const SourceLocation* loc)
{
    int64_t lineNr ;
    int64_t fileOffset;
    if(loc)
    {
        lineNr = loc->line;
        fileOffset =  loc->offset;
    }
    else
    {
        lineNr = files.top().lineNr;
        fileOffset =  fCurr - files.top().fileBase;
    }
    FILE_STATE fileState;
    manager->GetFileState(&files.top().fileId, &fileState);
    char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
    memcpy(pathBuffer, fileState.path, fileState.pathLen);
    pathBuffer[fileState.pathLen] = '\0';
    
    printf("%s:%ld:%ld %s\n", 
            pathBuffer, lineNr, fileOffset, msg);
}

void Lexer::LexConstant(Token *token, const SourceLocation *firstNum)
{
    RestoreLexerPointer();
    fCurr = files.top().fileBase + firstNum->offset;
    // set token 
    *token = {};
    token->type = TokenType::numeric_constant;
    token->location.id = files.top().fileId;
    token->location.line = files.top().lineNr;
    token->location.offset = fCurr - files.top().fileBase;
    
    const char* startOfConstant = fCurr;
    // set fCurr to first char in buffer
    DecimalType numType = CheckDecimalType();
    fCurr += numType.len;
 
    if(numType.type == dec_type_dec)
    {
        token->isDec = 1;
        while (fCurr < fEnd && IsDigit(*fCurr)){fCurr++;}
    }
    else if(numType.type == dec_type_oct)
    {
        token->isOct = 1;
        while (fCurr < fEnd && IsOctalDigit(*fCurr)){fCurr++;}
    }
    else if(numType.type == dec_type_hex)
    {
        token->isHex = 1;
        while (fCurr < fEnd && IsHexDigit(*fCurr)){fCurr++;}
    }
    else
    {
        token->isBin = 1;
        while (fCurr < fEnd && IsBinDigit(*fCurr)){fCurr++;}
    }
    
    if(fCurr == fEnd || LexIntegerSuffix() )
    {
        goto end_of_constant_lex;
    }

    if(numType.type == dec_type_dec)
    {
        bool isFloat = false;
        if(*fCurr == '.')
        {
            isFloat = true;
            fCurr++;
            while (fCurr < fEnd && IsDigit(*fCurr)){fCurr++;}
        }

        if(fCurr < fEnd && (*fCurr == 'e' || *fCurr == 'E') )
        {
            token->hasE = 1;
            isFloat = true;
            fCurr++;
            if(fCurr < fEnd && ((*fCurr == '+' || *fCurr == '-')))
            {
                fCurr++;
            }

            if(!(fCurr < fEnd && IsDigit(*fCurr)))
            {
                IssueWarning("Incorrectly typed hex float value", nullptr);
                exit(-1);
            }
            while (fCurr < fEnd && IsDigit(*fCurr)){fCurr++;}
        }
        if(isFloat)
        {
            token->isFloat = 1;
            LexFloatSuffix();
        }
    }
    else if(numType.type == dec_type_hex)
    {
        bool isFloat = false;
        if(*fCurr == '.')
        {
            isFloat = true;
            fCurr++;
            while (fCurr < fEnd && IsHexDigit(*fCurr)){fCurr++;}
        }


        if( (isFloat && fCurr >= fEnd) || (isFloat &&  fCurr < fEnd && *fCurr != 'p' ))
        {
            IssueWarning("Incorrectly typed hex float value", nullptr);
            exit(-1);
        }

        if(*fCurr == 'p')
        {
            token->hasP = 1;
            isFloat = true;
            fCurr++;

            if(fCurr < fEnd && ((*fCurr == '+' || *fCurr == '-')))
            {
                fCurr++;
            }

            if(!(fCurr < fEnd && IsDigit(*fCurr)))
            {
                IssueWarning("Incorrectly typed hex float value", nullptr);
                exit(-1);
            }

            while (fCurr < fEnd && IsDigit(*fCurr)){fCurr++;}
        }

        if(isFloat)
        {
            token->isFloat = 1;
            LexFloatSuffix();
        }
    }
end_of_constant_lex: 
    token->location.len = fCurr - startOfConstant;
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
    assert(!charsQueue.empty()  && !currLocations.empty());
    charsQueue.pop_front();
    currLocations.pop();
}

bool Lexer::SkipHorizontalWhiteSpace()
{
    bool skippedWhitespace = false;

    while (!charsQueue.empty() && IsHorizontalWhiteSpace(charsQueue.front()))
    {
        charsQueue.pop_front();
        currLocations.pop();
        skippedWhitespace = true;
    }
    

    // no remaining characters in queue, work on raw buffer
    if(charsQueue.empty())
    {
        while (fCurr < fEnd && IsHorizontalWhiteSpace(*fCurr))
        {
            fCurr++;
            skippedWhitespace = true;
        }
    }

    return skippedWhitespace;
}

int64_t Lexer::LexComment()
{
    // in order to correctly parse comment 
    // it is needed to move fCurr to offset after second \ and skip line
    // additionally both charHistory and charLocations need to be invalidated
    RestoreLexerPointer();

    int64_t commentLen = 0;
skip_loop:
    while (fCurr < fEnd && *fCurr != '\n')
    {
        commentLen++;
        fCurr++;
    }

    if(fCurr == fEnd && files.size() > 0)
    {
        ChangeLexedFile();
        fCurr = files.top().fileCurrent;
        fEnd = fEnd;
        goto skip_loop;
    }

    return commentLen;
}

int64_t Lexer::LexMultilineComment()
{
    RestoreLexerPointer();
    int64_t commentLen = 0;
skip_loop:
    while (fCurr + 1 < fEnd)
    {
        if(*fCurr == '\n')
        {
            files.top().lineNr++;
        }
        commentLen++;
        fCurr++;
        if(*fCurr == '*' && *(fCurr + 1) == '/')
        {
            break;
        }
    }
    fCurr += 2;

    if(fCurr == fEnd && files.size() > 0)
    {
        ChangeLexedFile();
        fCurr = files.top().fileCurrent;
        fEnd = fEnd;
        goto skip_loop;
    }

    return commentLen;
}

// returns packet int64_t b
// 0 - decimal notation, 1 - octal notation, 
// 2 - hex notation, 3 - binary notation
DecimalType Lexer::CheckDecimalType()
{
    uint64_t maxLen = fEnd - fCurr;
    if(maxLen >= 2  && fCurr[0] == '0' && 
      (fCurr[1] == 'x' || fCurr[1] == 'X') )
    {
        return {2, dec_type_hex};
    }
    if(maxLen >= 2  && fCurr[0] == '0' && 
      (fCurr[1] == 'b' || fCurr[1] == 'B') )
    {
        return {2, dec_type_bin};
    }
    else if(fCurr[0] == '0')
    {
        return {1, dec_type_oct};
    }
    return {1, dec_type_dec};
}

int64_t Lexer::IsUniversalChar(const char *c, size_t maxPossibleLen)
{
    if(maxPossibleLen >= 2 && c[0] == '\\')
    {
        // needed chars are \, u, x1, x2, x3, x4
        if(c[1] == 'u') 
        {
            if(maxPossibleLen < 6 || !IsHexDigit(c[2]) || !IsHexDigit(c[3]) || 
                             !IsHexDigit(c[4]) || !IsHexDigit(c[5])) 
            {
                printf("Incorrectly formed universal character name\n");
                exit(-1);
            }
            return 6; // size of UCN
        }
        // needed chars are \, U, x1, x2, x3, x4, x6, x7, x8, x9
        else if(c[1] == 'U' || !IsHexDigit(c[2]) || !IsHexDigit(c[3]) || 
                !IsHexDigit(c[4]) || !IsHexDigit(c[5]) || !IsHexDigit(c[6]) ||
                !IsHexDigit(c[7])|| !IsHexDigit(c[8])|| !IsHexDigit(c[9]))
        {
           if(maxPossibleLen < 10)
            {
            printf("Incorrectly formed universal character name\n"); 
            exit(-1);
            }

            return 10; // size of UCN
        }
    }

    return 0;
}

bool Lexer::IsNonDigit(const char *c, size_t maxPossibleLen)
{
    return IsAlpha(*c) || *c == '_' || (c[0] == '\\' && IsUniversalChar(c, maxPossibleLen) > 0);
}

int32_t Lexer::Lex(Token* token)
{
    *token = {};
    token->type = TokenType::none;
    token->location = {};
    token->skippedHorizWhitespace = SkipHorizontalWhiteSpace() ? 1 : 0;

    char C = GetCurrChar();
    SourceLocation loc = GetCurrLoc();
    token->location = loc;
    ConsumeChar();
    
    switch(C)
    {
    case '\0':
        token->type = TokenType::eof;
        break;
    case '\\':
        C = GetNextChar();
        if(C != '\n')
        {
            IssueWarning("Unexpected followup to \\", &loc);
            exit(-1);
        }
        ConsumeChar();
        files.top().lineNr++;
        token->type = TokenType::line_splice;
        break;
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
        else if(C == '*')
        {
            token->type = TokenType::comment;
            token->location.len = 2; // "//"
            ConsumeChar();
            token->location.len += LexMultilineComment();
            
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
        //add parsing numbers
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
    case '{': token->type = TokenType::l_brace;        
              if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '~': token->type = TokenType::tilde;          
              if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '}': token->type = TokenType::r_brace;        
              if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '[': token->type = TokenType::l_bracket;      
              if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case ']': token->type = TokenType::r_bracket;      
              if(token->location.len != 1){TrigraphWarning(&token->location);} break;
    case '(': token->type = TokenType::l_parentheses;  break;
    case ')': token->type = TokenType::r_parentheses;  break;
    case ';': token->type = TokenType::semicolon;      break;
    case ',': token->type = TokenType::comma;          break;
    case '?': token->type = TokenType::question_mark;  break;
    case '\'': 
        token->type = TokenType::character_literal; LexCharSequence(token, '\''); break;
    case '\"':     
        token->type = TokenType::string_literal; LexCharSequence(token, '\"'); break;
    case '\n':
        token->type = TokenType::new_line;
        token->location.len = 1;
        files.top().lineNr++;
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        LexConstant(token, &loc);
        break;
    default:
        LexIdentifier(token, &loc);
        break;
    }

    return 0;
}

int32_t Lexer::PushFile(FILE_ID id, int64_t offset, int64_t len)
{
    FILE_STATE state =  {};
    if(manager->GetFileState(&id, &state) < 0)
    {
        return -2;
    }

    FilePos newFile = {};
    newFile.fileId = id;
    newFile.lineNr = 1;
    newFile.fileBase = state.fileData;
    newFile.fileCurrent = state.fileData;
    newFile.fileEnd = state.fileData + state.fileSize;
    newFile.fileOffset = 0;
    if(offset != -1)
    { 
        if( newFile.fileBase + offset > newFile.fileEnd) {return -3;}
        newFile.fileCurrent+= offset;
        newFile.fileOffset = offset;
    }
    if(len != -1)
    {
        if(newFile.fileCurrent + len >  newFile.fileEnd){ return -4;}
        newFile.fileEnd = newFile.fileCurrent + len;
    }
    // save current context
    if(fCurr)
    {
        files.top().fileCurrent = fCurr;
    }
    // reload all the pointers
    files.push(newFile);
    fCurr = files.top().fileCurrent;
    fEnd = files.top().fileEnd;
    return 0;
}

int32_t Lexer::PopFile()
{
    if(files.size() == 0)
    {
        return -1;
    }
    if(files.size() == 1)
    {
        lastFileLoc = ConstructLocation(files.top(), fCurr, 0);
    }
    files.pop();
    if(files.size() > 0)
    {
        fCurr = files.top().fileCurrent;
        fEnd = files.top().fileEnd;
    }
    else
    {
        fCurr = 0;
        fEnd = 0;
    }
    return 0;
}

SourceLocation Lexer::CurrentSourceLocation()
{ 
    return ConstructLocation(files.top(), fCurr, 1);
}

/*
    This function is used to bypass char queue system
    and keep everything consistent
*/
std::vector<Token> Lexer::LexFile(FILE_ID id, int64_t offset, int64_t len)
{
    RestoreLexerPointer();

    std::vector<Token> out;
    Token token;
    PushFile(id, offset, len);
    Lex(&token);
    while (token.type != TokenType::eof)
    {
        out.push_back(token);
        Lex(&token);
    }
    PopFile();
    return out;
}

SourceLocation Lexer::ConstructLocation(const FilePos &filePos, const char *fileCurr, int64_t len)
{
    SourceLocation loc;
    loc.id = filePos.fileId;
    loc.offset = fileCurr - filePos.fileBase;
    loc.line = filePos.lineNr;
    loc.len = len;
    return loc;
}

void Lexer::PrepareKeywordMap()
{
    keywordsMap[std::string_view(kewordStrings[0])]  = TokenType::kw_auto;
    keywordsMap[std::string_view(kewordStrings[1])]  = TokenType::kw_break;
    keywordsMap[std::string_view(kewordStrings[2])]  = TokenType::kw_case;
    keywordsMap[std::string_view(kewordStrings[3])]  = TokenType::kw_char;

    keywordsMap[std::string_view(kewordStrings[4])]  = TokenType::kw_const;
    keywordsMap[std::string_view(kewordStrings[5])]  = TokenType::kw_continue;
    keywordsMap[std::string_view(kewordStrings[6])]  = TokenType::kw_default;
    keywordsMap[std::string_view(kewordStrings[7])]  = TokenType::kw_do;

    keywordsMap[std::string_view(kewordStrings[8])]  = TokenType::kw_double;
    keywordsMap[std::string_view(kewordStrings[9])]  = TokenType::kw_else;
    keywordsMap[std::string_view(kewordStrings[10])] = TokenType::kw_enum;
    keywordsMap[std::string_view(kewordStrings[11])] = TokenType::kw_extern;

    keywordsMap[std::string_view(kewordStrings[12])] = TokenType::kw_float;
    keywordsMap[std::string_view(kewordStrings[13])] = TokenType::kw_for;
    keywordsMap[std::string_view(kewordStrings[14])] = TokenType::kw_goto;     
    keywordsMap[std::string_view(kewordStrings[15])] = TokenType::kw_if;

    keywordsMap[std::string_view(kewordStrings[16])] = TokenType::kw_inline;
    keywordsMap[std::string_view(kewordStrings[17])] = TokenType::kw_int;
    keywordsMap[std::string_view(kewordStrings[18])] = TokenType::kw_long;
    keywordsMap[std::string_view(kewordStrings[19])] = TokenType::kw_register;

    keywordsMap[std::string_view(kewordStrings[20])] = TokenType::kw_restrict;
    keywordsMap[std::string_view(kewordStrings[21])] = TokenType::kw_return;
    keywordsMap[std::string_view(kewordStrings[22])] = TokenType::kw_short;
    keywordsMap[std::string_view(kewordStrings[23])] = TokenType::kw_signed;

    keywordsMap[std::string_view(kewordStrings[24])] = TokenType::kw_sizeof;
    keywordsMap[std::string_view(kewordStrings[25])] = TokenType::kw_static;
    keywordsMap[std::string_view(kewordStrings[26])] = TokenType::kw_struct;
    keywordsMap[std::string_view(kewordStrings[27])] = TokenType::kw_switch;

    keywordsMap[std::string_view(kewordStrings[28])] = TokenType::kw_typedef;
    keywordsMap[std::string_view(kewordStrings[29])] = TokenType::kw_union;
    keywordsMap[std::string_view(kewordStrings[30])] = TokenType::kw_unsigned;
    keywordsMap[std::string_view(kewordStrings[31])] = TokenType::kw_void;

    keywordsMap[std::string_view(kewordStrings[32])] = TokenType::kw_volatile;
    keywordsMap[std::string_view(kewordStrings[33])] = TokenType::kw_while;
    keywordsMap[std::string_view(kewordStrings[34])] = TokenType::kw_bool;
    keywordsMap[std::string_view(kewordStrings[35])] = TokenType::kw_complex;
    keywordsMap[std::string_view(kewordStrings[36])] = TokenType::kw_imaginary;

    // preprocessor keywords MUST be resolved by preprocessor because they
    // can be used by a user outside of #macro formula
    keywordsMap[std::string_view(kewordStrings[37])] = TokenType::pp_include;
    keywordsMap[std::string_view(kewordStrings[38])] = TokenType::pp_define;
    keywordsMap[std::string_view(kewordStrings[39])] = TokenType::pp_ifdef;
    keywordsMap[std::string_view(kewordStrings[40])] = TokenType::pp_ifndef;
    keywordsMap[std::string_view(kewordStrings[41])] = TokenType::pp_elif;
    keywordsMap[std::string_view(kewordStrings[42])] = TokenType::pp_endif;
    keywordsMap[std::string_view(kewordStrings[43])] = TokenType::pp_line;
    keywordsMap[std::string_view(kewordStrings[44])] = TokenType::pp_error;
    keywordsMap[std::string_view(kewordStrings[45])] = TokenType::pp_pragma;
    keywordsMap[std::string_view(kewordStrings[46])] = TokenType::pp_undef;
    keywordsMap[std::string_view(kewordStrings[47])] = TokenType::pp_defined;
    
}

bool Lexer::IsDigit(const char &c)
{
    return '0' <= c  && c <= '9';
}

bool Lexer::IsOctalDigit(const char &c)
{
    return '0' <= c  && c <= '7';
}

bool Lexer::IsBinDigit(const char &c)
{
    return '0' <= c  && c <= '1';
}

bool Lexer::LexIntegerSuffix()
{
    RestoreLexerPointer();
    if(fCurr == fEnd)
    {
        return false;
    }

    if( *fCurr == 'U' || *fCurr == 'u')
    {
        fCurr++;
        if(fCurr < fEnd &&( *fCurr == 'l' || *fCurr == 'L')){fCurr++;} // ul suffix 
        if(fCurr < fEnd &&( *fCurr == 'l' || *fCurr == 'L')){fCurr++;} // ull suffix
        return true;
    }
    else if(*fCurr == 'L' || *fCurr == 'l')
    {
        fCurr++;
        if(fCurr < fEnd &&( *fCurr == 'u' || *fCurr == 'U'))
        {
            fCurr++; 
            return true; // lu suffix
        } 
        if(fCurr < fEnd &&( *fCurr == 'l' || *fCurr == 'L')){fCurr++;} // ll suffix 
        if(fCurr < fEnd &&( *fCurr == 'u' || *fCurr == 'U')){fCurr++;} // llu suffix 
        return true;
    }
    return false;
}

bool Lexer::LexFloatSuffix()
{
    RestoreLexerPointer();
    if(fCurr >= fEnd)
    {
        return false;
    }

    if(*fCurr == 'f' || *fCurr == 'F' || 
       *fCurr == 'l' || *fCurr == 'L' )
    {
        fCurr++;
        return true;
    }

    return false;
}

bool Lexer::IsAlpha(const char &c)
{
    return ('A' <= c  && c <= 'Z') || ('a' <= c  && c <= 'z' );
}

bool Lexer::IsHexDigit(const char &c)
{
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f')  ||('A' <= c && c <= 'F') ;
}

bool Lexer::IsAlphaDigitFloor(const char &c)
{
    return IsDigit(c) || IsAlpha(c) || c == '_';
}
