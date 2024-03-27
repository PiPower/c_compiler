#include "../../include/frontend/scanner.hpp"

Scanner::Scanner(const char *sourceCode)
:
currentToken(0)
{
    unsigned int i =0;
    unsigned int line = 0;
    while (sourceCode[i] != '\0')
    {
        
        i++;
    }
    
    tokenStream.push_back( Token{TokenType::EOF, line} );
}