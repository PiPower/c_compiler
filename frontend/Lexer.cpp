#include "Lexer.hpp"
#include <cassert>
#include <stdio.h>
#include <string.h>
Lexer::Lexer(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
mainFile(mainFile), manager(manager), opts(opts)
{
    assert(manager != nullptr);

    FilePos initialFile;
    manager->GetFileId(mainFile.path, mainFile.pathLen, &initialFile.fileId);
    initialFile.lineNr = 1;
    initialFile.fileBase = mainFile.fileData;
    initialFile.fileCurr = mainFile.fileData;
    initialFile.fileEnd = mainFile.fileData + mainFile.fileSize;
    files.push(initialFile);

    fCurr = initialFile.fileCurr;
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

char Lexer::GetNextChar()
{
    if(IsSimpleChar(*fCurr))
    {
        char c = *fCurr;
        fCurr++;
        return c;
    }
    return GetCharAndSizeSlow();
}

char Lexer::GetCharAndSizeSlow()
{
    uint64_t remainingChars = fEnd - fCurr;
    if(*fCurr == '\\')
    {
slash:
        if(remainingChars > 0 && !IsWhiteSpace(*(fCurr + 1)))
        {
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

    if(fCurr == fEnd)
    {
        token->type = TokenTypes::eof;
        token->location = {};
        return 0;
    }

    char C = GetNextChar();

    return 0;
}
