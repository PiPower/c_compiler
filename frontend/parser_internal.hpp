#ifndef PARSER_INTERNAL_HPP
#define PARSER_INTERNAL_HPP

#include "parser.hpp"
#include "node_allocator.hpp"
#include <setjmp.h>
#include <string>

// if parsing was succesfull yet returns null return PARSER_SUCC 
#define PARSER_SUCC  ((AstNode*)0xFFFFFFFFFFFFFFFF)
#define ALLOCATE_NODE(parser) allocateNode((parser)->allocator)
#define CONSUME_TOKEN(parser, type) (parser)->scanner->consume(type)
#define GET_TOKEN(parser) (parser)->scanner->getToken()
#define POP_TOKEN(parser) GET_TOKEN( (parser) )
#define PEEK_TOKEN(parser) (parser)->scanner->peekToken()

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

// parser for statements
// ----------------------------------------------

AstNode* parseStatement(ParserState* parser);


// parser for declarations
// ----------------------------------------------

// if returns nullptr no decleration has been detected
AstNode* parseDeclaration(ParserState* parser);
AstNode* parseInitDeclList(ParserState* parser, const std::string* typeName);
AstNode* parseDeclarator(ParserState* parser);
AstNode* parseInitializer(ParserState* parser);
std::string* parseDeclSpec(ParserState* parser);
AstNode* parseDirectDeclarator(ParserState* parser);
// parser for expressions
// ----------------------------------------------

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



#endif