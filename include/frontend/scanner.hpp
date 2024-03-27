#ifndef SCANNER
#define SCANNER
#include "token_types.hpp"
#include <vector>


class Scanner
{

public:
    Scanner(const char* sourceCode);
private:
    std::vector<Token> tokenStream;
    unsigned int currentToken;
};

#endif