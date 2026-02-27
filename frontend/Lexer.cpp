#include "Lexer.hpp"


Lexer::Lexer(FILE_STATE mainFile, FileManager* manager)
:
m_mainFile(mainFile), m_manager(manager)
{
    FilePos initialFile;
    m_manager->GetFileId(m_mainFile.path, m_mainFile.pathLen, &initialFile.fileId);;
    initialFile.fileBase = m_mainFile.fileData;
    initialFile.fileCurr = m_mainFile.fileData;
    initialFile.fileEnd = m_mainFile.fileData + m_mainFile.fileSize;
    m_files.push(initialFile);

    m_fCurr = initialFile.fileCurr;
    m_fEnd = initialFile.fileEnd;
}

bool Lexer::IsHorizontalWhiteSpace(char C)
{
    return C == ' ' || C == '\v' || C == '\r' || C == '\f';
}

char Lexer::GetNextChar()
{
    
    return 0;
}

int32_t Lexer::Lex(Token* token)
{
    while (IsHorizontalWhiteSpace(*m_fCurr) && m_fCurr < m_fEnd)
    {
        m_fCurr++;
    }
    

    return 0;
}
