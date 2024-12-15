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
    // assignment flow control;
    bool isParsingAssignment;;
    jmp_buf assignmentJmp;
    AstNode* jmpHolder;
    // error handling
    jmp_buf jmpBuff;
    uint32_t errCode;
    char* errorMessage;
    unsigned int errorMessageLen;
};

#define ALLOCATE_NODE(parser) allocateNode((parser)->allocator)

// parser for expressions
AstNode* parseExpression(ParserState* parser);
AstNode* assignmentExpression(ParserState *parser);
AstNode* conditionalExpression(ParserState *parser);
AstNode* logOrExpression(ParserState *parser);
AstNode* logAndExpression(ParserState *parser);
AstNode* incOrExpression(ParserState *parser);
AstNode* excOrExpression(ParserState *parser);
AstNode* AndExpression(ParserState *parser);
AstNode* equalityExpression(ParserState *parser);
AstNode* relationalExpression(ParserState *parser);
AstNode* shiftExpression(ParserState *parser);
AstNode* additiveExpression(ParserState *parser);
AstNode* multiplicativeExpression(ParserState *parser);
AstNode* castExpression(ParserState *parser);
AstNode* unaryExpression(ParserState *parser);
AstNode* postfixExpression(ParserState *parser);
AstNode* primaryExpression(ParserState *parser);
// parser for statements
AstNode* parseStatement(ParserState* parser);



#endif