#ifndef PARSER_INTERNAL_HPP
#define PARSER_INTERNAL_HPP

#include "parser.hpp"
#include "node_allocator.hpp"
#include <setjmp.h>

struct ParserState
{
    Scanner* scanner;
    SymbolTable* symtab;
    NodeAllocator* allocator;
    // error handling
    jmp_buf jmpBuff;
    uint32_t errCode;
    char* errorMessage;
    unsigned int errorMessageLen;
};

typedef AstNode* ( *parseFunctionPtr)(ParserState* parser);

// parser for expressions
AstNode* parseExpression(ParserState* parser);
AstNode* unaryExpression(ParserState *parser);
AstNode* postfixExpression(ParserState *parser);
AstNode* primaryExpression(ParserState *parser);
// parser for statements
AstNode* parseStatement(ParserState* parser);

//utils
void triggerParserError(ParserState* parser,
                         int value, 
                         const char* format, ...);
AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction,
                    AstNode* root, 
                    const std::vector<TokenType>& types);
NodeType tokenMathTypeToNodeType(const Token& token);

#endif