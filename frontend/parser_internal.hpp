#ifndef PARSER_INTERNAL_HPP
#define PARSER_INTERNAL_HPP
#include "parser.hpp"
#include <setjmp.h>

struct ParserState
{
    Scanner* scanner;
    SymbolTable* symtab;
    std::vector<AstNode *> nodes;
    // error handling
    jmp_buf jmpBuff;
    uint32_t errCode;
    char* errorMessage;
    unsigned int errorMessageLen;
};

// parser for expressions
AstNode* parseExpression(ParserState* parser);
AstNode* primaryExpression(ParserState *parser);
// parser for statements
AstNode* parseStatement(ParserState* parser);



#endif