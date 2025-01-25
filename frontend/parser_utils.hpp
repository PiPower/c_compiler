#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H
#include "scanner.hpp"
#include "../symbol_table.hpp"
// math ops  
//   AMPRESAND, STAR, PLUS, MINUS, TILDE, BANG, SLASH, 
//   PERCENT, LESS, GREATER,  L_SHIFT,
//    R_SHIFT, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, BANG_EQUAL,
//   PIPE, CARET, DOUBLE_AMPRESAND, DOUBLE_PIPE, QUESTION_MARK,
//    PLUS_PLUS, MINUS_MINUS,

enum class NodeType
{
    NONE, 
    IDENTIFIER,
    CONSTANT,
    DECLARATION,
    CONDITIONAL_EXPRESSION,
//basic math ops
    MULTIPLY, DIVIDE, DIVIDE_MODULO,
    ADD, SUBTRACT, L_SHIFT, R_SHIFT,
    AND, EXC_OR, OR, COMPLIMENT,
// prefix ops
    PRE_INC, POST_INC, PRE_DEC, POST_DEC,
    PLUS, MINUS,
//logical ops    
    LOG_AND, LOG_OR, LOG_NOT,
// comparisons
    LESS, GREATER, LESS_EQUAL, 
    GREATER_EQUAL, EQUAL, NOT_EQUAL,
// assignment nodes 
    ASSIGNMENT, MUL_ASSIGNMENT, DIV_ASSIGNMENT, MOD_ASSIGNMENT,
    ADD_ASSIGNMENT, SUB_ASSIGNMENT, L_SHIFT_ASSIGNMENT, R_SHIFT_ASSIGNMENT,
    AND_ASSIGNMENT, EXC_OR_ASSIGNMENT, OR_ASSIGNMENT,
// function
    FUNCTION_DECL, FUNCTION_DEF, ARGS_EXPR_LIST,
    FUNCTION_CALL, PARAMETER_LIST, IDENTIFIER_LIST,
// statement
    BLOCK, IF, ELSE, FOR_LOOP, WHILE_LOOP, DO_WHILE_LOOP,
    RETURN, CONTINUE, BREAK,
// misc
    NODE_EMPTY, DECLARATION_LIST, TYPE_PLACEHOLDER, INITALIZER,
    POINTER, DREF_PTR, ACCESS, CAST, GET_ADDR, PTR_ACCESS,
    DECLARATOR
};

struct TypePair
{
    std::string* type;
    std::string* qualifiers;
};

struct ParserState;
struct AstNode;


typedef AstNode* ( *parseFunctionPtr)(ParserState* parser);

void triggerParserError(ParserState* parser,
                         int value, 
                         const char* format, ...);
void triggerParserWarning(ParserState* parser,
                         const char* format, ...);

AstNode* fillSymtab(AstNode* root);
NodeType tokenMathTypeToNodeType(const Token& token);
uint8_t getTypeGroup(ParserState* parser, const std::string* name);
std::string* copyStrongerType(ParserState* parser, const std::string* t1, const std::string* t2);
std::string* resolveImpConv(ParserState* parser, 
                                           const std::string* t1, 
                                           const std::string* t2, 
                                            uint8_t g1, 
                                            uint8_t g2);
static inline NodeType assignementTokenToNodeType(const Token& token)
{
    return (NodeType) ((int)token.type - (int)TokenType::EQUAL + (int)NodeType::ASSIGNMENT) ;
}
// fills and checks symbol table
// removes nodes that are more abstact
std::vector<AstNode*> processDeclarationTree(AstNode* root, ParserState* parser);
AstNode* processVariable(AstNode *root, ParserState *parser);
AstNode* processFunction(AstNode* root, ParserState* parser);
SymbolType* defineTypeSymbol(ParserState *parser, const std::string* typeName);
std::string* generateAnonymousStructName(ParserState *parser);
void addParameterToStruct(ParserState *parser, 
                            SymbolType *typeVar,
                            AstNode *paramNode);
SymbolType* getSymbolType(ParserState *parser, const std::string* name);
uint16_t getTypeAffiliation(ParserState *parser, std::string* name);
TypePair decodeType(const std::string* encodedType);
AstNode* unwindReturnType(ParserState *parser, AstNode* retPtr);
#endif