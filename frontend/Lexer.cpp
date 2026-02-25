#include "Lexer.hpp"


Lexer::Lexer(FILE_STATE mainFile, FileManager* manager)
:
m_currFile(mainFile), m_manager(manager)
{

}
