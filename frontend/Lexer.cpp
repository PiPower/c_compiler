#include "Lexer.hpp"
#include <cassert>
#include <stdio.h>

Lexer::Lexer(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
m_mainFile(mainFile), m_manager(manager), m_opts(opts)
{
    assert(m_manager != nullptr);

    FilePos initialFile;
    m_manager->GetFileId(m_mainFile.path, m_mainFile.pathLen, &initialFile.fileId);
    initialFile.fileBase = m_mainFile.fileData;
    initialFile.fileCurr = m_mainFile.fileData;
    initialFile.fileEnd = m_mainFile.fileData + m_mainFile.fileSize;
    m_files.push(initialFile);

    m_fCurr = initialFile.fileCurr;
    m_fEnd = initialFile.fileEnd;
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
    if(IsSimpleChar(*m_fCurr))
    {
        char c = *m_fCurr;
        m_fCurr++;
        return c;
    }
    return GetCharAndSizeSlow();
}

char Lexer::GetCharAndSizeSlow()
{
    uint64_t remainingChars = m_fEnd - m_fCurr;
    if(*m_fCurr == '\\')
    {
slash:
        if(remainingChars > 0 && !IsWhiteSpace(*(m_fCurr + 1)))
        {
            m_fCurr++;
            return '\\';
        }
        //TODO add support for escaped-newline
        m_fCurr++;
        return '\\';
    }
    
    // process trigraph
    if(m_opts->trigraphs_enabled && remainingChars >= 2 && m_fCurr[0] == '?' && m_fCurr[1] == '?')
    {
        char out = '\0';
        switch (m_fCurr[2])
        {
        case '/':
            remainingChars -= 3;
            m_fCurr += 3;
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
        default: return *m_fCurr;
        }
        if(m_opts->trigraphs_set == 0)
        {
            printf("WARNING: Trigraph detected in source code, in order to hide warning use flag -ftrigraphs\n");
        }
        m_fCurr += 3;
        return out;
    }
    return *m_fCurr;
}

void Lexer::SkipHorizonthalWhiteSpace()
{
    while (m_fCurr < m_fEnd && IsHorizontalWhiteSpace(*m_fCurr))
    {
        m_fCurr++;
    }
}

int32_t Lexer::Lex(Token* token)
{

    SkipHorizonthalWhiteSpace();

    if(m_fCurr == m_fEnd)
    {
        token->type = TokenTypes::eof;
        token->location = {};
        return 0;
    }

    char C = GetNextChar();

    return 0;
}
