#include "parser_utils.hpp"
#include "parser.hpp"
#include "parser_internal.hpp"
#include <stdarg.h>
#include <string.h>

using namespace std;
AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction, 
                    AstNode* root, 
                    const vector<TokenType>& types)
{
   return parseLoop(parser, parsingFunction, root, types.data(), types.size());
}

AstNode *parseLoop(ParserState *parser,
                    parseFunctionPtr parsingFunction, 
                    AstNode *root, 
                    const TokenType *types, 
                    const uint64_t typesCount)
{
    while (parser->scanner->currentTokenOneOf(types, typesCount))
    {
        Token operatorToken = parser->scanner->getToken();
        AstNode* parent = ALLOCATE_NODE(parser);
        parent->nodeType = tokenMathTypeToNodeType(operatorToken);
        AstNode* right = parsingFunction(parser);

        parent->children.push_back(root);
        parent->children.push_back(right);
        root = parent;
    }

    return root;
}

void triggerParserError(ParserState* parser, int value, const char* format, ...)
{
    parser->errCode = 1;
    int len = snprintf(parser->errorMessage, parser->errorMessageLen, 
    "Line %u: ", parser->scanner->line);

    va_list args;
    va_start(args, format);
    len = vsnprintf(parser->errorMessage + len, parser->errorMessageLen, format, args);
    va_end(args);
    if(len <  0)
    {
        printf("Error buffer is too small \n");
    }
    longjmp(parser->jmpBuff, value);
}

NodeType tokenMathTypeToNodeType(const Token& token)
{
    switch (token.type)
    {
    case TokenType::PLUS :
        return NodeType::ADD;
    case TokenType::MINUS :
        return NodeType::SUBTRACT;
    case TokenType::SLASH :
        return NodeType::DIVIDE;
    case TokenType::PERCENT :
        return NodeType::DIVIDE_MODULO;
    case TokenType::STAR :
        return NodeType::MULTIPLY;
    case TokenType::L_SHIFT :
        return NodeType::L_SHIFT;
    case TokenType::R_SHIFT :
        return NodeType::R_SHIFT;
    case TokenType::LESS :
        return NodeType::LESS;
    case TokenType::GREATER :
        return NodeType::GREATER;
    case TokenType::LESS_EQUAL :
        return NodeType::LESS_EQUAL;
    case TokenType::GREATER_EQUAL :
        return NodeType::GREATER_EQUAL;
    case TokenType::EQUAL_EQUAL :
        return NodeType::EQUAL;
    case TokenType::BANG_EQUAL :
        return NodeType::NOT_EQUAL;
    case TokenType::PIPE :
        return NodeType::OR;
    case TokenType::AMPRESAND :
        return NodeType::AND;
    case TokenType::CARET :
        return NodeType::EXC_OR;
    case TokenType::DOUBLE_AMPRESAND :
        return NodeType::LOG_AND;
    case TokenType::DOUBLE_PIPE :
        return NodeType::LOG_OR;
    }

    fprintf(stdout, "unexpected token at line %d \n", token.line);
    exit(-1);
}

std::vector<AstNode*> processDeclarationTree(AstNode *root, ParserState* parser)
{
    vector<AstNode*> out;
    if(root->nodeType == NodeType::FUNCTION_DEF)
    {

    }
    else if(root->nodeType == NodeType::DECLARATION_LIST)
    {
        std::vector<AstNode*> children = std::move(root->children);
        freeNode(parser->allocator, root);
        for(AstNode* declarator : children)
        {
            AstNode* processed = nullptr;
            if(declarator->nodeType == NodeType::FUNCTION_DECL ||
                declarator->nodeType == NodeType::FUNCTION_DEF )
            {
                processed = processFunction(declarator, parser);
            }
            else
            {
                processed = processVariable(declarator, parser);
            }

            if(processed != nullptr)
            {
                out.push_back(processed);
            }
        }
    }
    return out;
}

AstNode* processVariable(AstNode *root, ParserState *parser)
{
    AstNode* initializer = nullptr;
    if(root->children.size() > 0)
    {
        initializer = root->children[0];
    }
    return nullptr;
}

AstNode *processFunction(AstNode *root, ParserState *parser)
{
    AstNode* args = root->children[0];
    SymtabIter iter = GET_SYMBOL(parser, *(string*)root->data);
    SymbolFunction * fn;
    if(iter != SYMTAB_CEND(parser))
    {
        fn = (SymbolFunction*)(*iter).second;
        if(isSetDefinedAttr(fn) &&  root->nodeType == NodeType::FUNCTION_DEF)
        {
            triggerParserError(parser, 1, "Redefiniction of function named %s", root->data->c_str());
        }
        // check if args are consistent
        if( *root->type != *fn->retType)
        {
            triggerParserError(parser, 1, "Redeclared function %s has different return type than before\n", root->data->c_str());
        }

        if( args->children.size() != fn->argTypes.size())
        {
            triggerParserError(parser, 1, "Redeclared function %s has different arguments than before\n", root->data->c_str());
        }
        for(int i = 0; i < args->children.size(); i++ )
        {
            if(*(args->children[i]->type) != *(fn->argTypes[i]) )
            {
                triggerParserError(parser, 1, "Redeclared function %s has different arguments than before\n", root->data->c_str());
            }
        }
    }
    else
    {
        fn = new SymbolFunction;
        fn->attributes = 0;
        fn->type = SymbolClass::FUNCTION;
        fn->retType = root->type;
        SET_SYMBOL(parser, *root->data, (Symbol*)fn);

        if(root->nodeType == NodeType::FUNCTION_DEF)
        {
            setDefinedAttr(fn);
        }

        for( AstNode* param : args->children)
        {
            fn->argTypes.push_back(param->type);
        }
    }
    // if function definition retain function declaration
    // otherwise free them
    if(root->nodeType == NodeType::FUNCTION_DEF)
    {
        return root;
    }

    for(AstNode* arg : args->children)
    {
        freeNode(parser->allocator, arg);
    }
    freeNode(parser->allocator, args);
    freeNode(parser->allocator, root);
    return nullptr;
}
