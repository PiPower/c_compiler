#include "Preprocessor.hpp"



Preprocessor::Preprocessor(FILE_STATE m_mainFile, FileManager *manager)
:
m_lexer(m_mainFile, manager)
{
}
