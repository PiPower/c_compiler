#include "parser_utils.hpp"
#include "parser.hpp"
#include "parser_internal.hpp"
#include <stdarg.h>
#include <string.h>
#include <random>

using namespace std;

void triggerParserError(ParserState* parser, int value, const char* format, ...)
{
    parser->errCode = 1;
    int len = snprintf(parser->errorMessage, parser->errorMessageLen, 
    "Error line %u: ", parser->scanner->line);

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

void triggerParserWarning(ParserState *parser, const char *format, ...)
{
    printf("Warning line %u: ",  parser->scanner->line);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
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

uint8_t getTypeGroup(ParserState *parser,  AstNode *typeNode)
{   
    Symbol* sym = GET_SYMBOL(parser, *typeNode->data);
    if(!sym)
    {
        triggerParserError(parser, 1, "type named %s does not exist\n", typeNode->data->c_str());
    }
    if(sym->symClass != SymbolClass::TYPE)
    {
        triggerParserError(parser, 1, "Implementation error: %s is not a typename\n", typeNode->data->c_str());
    }
    SymbolType* symType = (SymbolType*)sym;

    if( (symType->affiliation & 0x0F) > 0)
    {
        return SIGNED_INT_GROUP;
    }
    if( (symType->affiliation & (0x0F << 4)) > 0)
    {
        return UNSIGNED_INT_GROUP;
    }
    if( (symType->affiliation &  (0x0F << 8)) > 0)
    {
        return FLOAT_GROUP;
    }
        
    return SPECIAL_GROUP;
}
// n1 and n2 are assumed to be from the same type group
std::string *copyStrongerType(ParserState* parser, AstNode *n1, AstNode *n2)
{
    Symbol* sym1 = GET_SYMBOL(parser, *n1->data);
    Symbol* sym2 = GET_SYMBOL(parser, *n2->data);
    if(!sym1)
    {
        triggerParserError(parser, 1, "type named %s does not exist\n", n1->data->c_str());
    }
    if(sym1->symClass != SymbolClass::TYPE)
    {
        triggerParserError(parser, 1, "Implementation error: %s is not a typename\n", n1->data->c_str());
    }
    if(!sym2)
    {
        triggerParserError(parser, 1, "type named %s does not exist\n", n1->data->c_str());
    }
    if(sym2->symClass != SymbolClass::TYPE)
    {
        triggerParserError(parser, 1, "Implementation error: %s is not a typename\n", n1->data->c_str());
    }
    SymbolType* symType1 = (SymbolType*)sym1;
    SymbolType* symType2 = (SymbolType*)sym2;
    if(symType1->affiliation > symType2->affiliation)
    {
        return new string(*n1->data);
    }

    return new string(*n2->data);
}

std::vector<AstNode*> processDeclarationTree(AstNode *root, ParserState* parser)
{
    vector<AstNode*> out;
    if(root == nullptr)
    {
        return {};
    }
    if(root->nodeType == NodeType::FUNCTION_DEF)
    {
        if(parser->symtab->scopeLevel > 0)
        {
            triggerParserError(parser, 1, "Defining a function inside another function is not allowed\n");
        }

        AstNode* processed = processFunction(root, parser);
        out.push_back(processed);
        return out;
    }


    std::vector<AstNode*> children = std::move(root->children);
    freeNode(parser->allocator, root);
    for(AstNode* declarator : children)
    {
        AstNode* processed = nullptr;
        if(declarator->nodeType == NodeType::FUNCTION_DECL)
        {
            processed = processFunction(declarator, parser);
        }
        else
        {
            if(declarator->nodeType == NodeType::POINTER)
            {
                AstNode* ptr = declarator;
                declarator = ptr->children[0];
                if( ptr->children.size() == 2)
                {
                    declarator->children.push_back( ptr->children[1]);
                }
                declarator->type = ptr->type;
                ptr->type = nullptr;
                *declarator->type += '#'; //mark ptr attributes
                *declarator->type += *ptr->data;
                FREE_NODE(parser, ptr);
            }
            processed = processVariable(declarator, parser);
        }

        if(processed != nullptr)
        {
            out.push_back(processed);
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

    uint64_t scope = 0;
    Symbol* sym = GET_SYMBOL_EX(parser, *(string*)root->data, &scope);
    SymbolVariable* var = nullptr;
    if(sym && scope == parser->symtab->scopeLevel)
    {
        if(sym->symClass != SymbolClass::VARIABLE)
        {
            triggerParserError(parser, 1, "Identifier %s is not a variable\n", root->data->c_str());
        }
        var = (SymbolVariable*)sym;
        if(isSetDefinedAttr(var))
        {
            triggerParserError(parser, 1, "Redefinition of variable %s\n", root->data->c_str());
        }
    }
    else
    {
        var = new SymbolVariable();
        var->symClass = SymbolClass::VARIABLE;
        var->varType = root->type;
        root->type = nullptr;
        SET_SYMBOL(parser, *root->data, (Symbol*)var);
        setDefinedAttr(var);
        if(initializer)
        {
            return root;
        }
    }
    // if not initializer we can free node
    freeNode(parser->allocator, root);
    return nullptr;
}

AstNode *processFunction(AstNode *root, ParserState *parser)
{
    AstNode* args = root->children[0];
    Symbol* sym = GET_SYMBOL(parser, *(string*)root->data);
    SymbolFunction * fn;
    if(sym)
    {
        if( sym->symClass != SymbolClass::FUNCTION)
        {
            triggerParserError(parser, 1, "Identifier %s is not a function", root->data->c_str());
        }

        fn = (SymbolFunction*)sym;
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
        fn->fnStackSize = 0;
        fn->symClass = SymbolClass::FUNCTION;
        fn->retType = root->type;
        root->type = nullptr;
        SET_SYMBOL(parser, *root->data, (Symbol*)fn);

        if(root->nodeType == NodeType::FUNCTION_DEF)
        {
            setDefinedAttr(fn);
        }

        for( AstNode* param : args->children)
        {
            fn->argTypes.push_back(param->type);
            param->type = nullptr;
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

SymbolType *defineTypeSymbol(ParserState *parser, const std::string* typeName)
{
    SymbolType* symType = (SymbolType*)GET_SYMBOL(parser, *typeName);
    if(symType)
    {
        if(symType->symClass != SymbolClass::TYPE)
        {
            AstNode* ptrHandle = ALLOCATE_NODE(parser);
            ptrHandle->data = const_cast< std::string *>(typeName);
            triggerParserError(parser, 1, "symbol \"%s\" is not a type\n", typeName->c_str());
        }
        if(isSetDefinedAttr(symType))
        {
            AstNode* ptrHandle = ALLOCATE_NODE(parser);
            ptrHandle->data = const_cast< std::string *>(typeName);
            triggerParserError(parser, 1, "symbol \"%s\" is already defined\n", typeName->c_str());
        }
        setDefinedAttr(symType);
    }
    else
    {
        symType = new SymbolType();
        symType->symClass = SymbolClass::TYPE;
        symType->affiliation = STRUCT_GR;
        setDefinedAttr(symType);
        SET_SYMBOL(parser, *typeName, (Symbol*)symType);
    }
    return symType;
}

std::string *generateAnonymousStructName(ParserState *parser)
{
    string* out = new string("<anonymous>");

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<double> dist(-1000000000000, 1000000000000);
    while (true)
    {
        double seed = dist(e2);
        char *ptr = (char *)&seed;
        for(int i =0; i< sizeof(double); i++)
        {
            *out+=ptr[i];
        }

        Symbol* sym = GET_SYMBOL(parser, *out);
        if(sym)
        {
            (*out).erase(11, 8);
        }
        else
        {
            return out;
        }
    }
    
    return nullptr;
}

void addParameterToStruct(ParserState *parser, 
                            SymbolType *typeVar,
                            AstNode *typeNode, 
                            const std::string* type)
{
    if(typeNode->nodeType != NodeType::IDENTIFIER && 
        typeNode->nodeType != NodeType::POINTER)
    {
        triggerParserError(parser, 1, "Incorrect declaration of name of the type\n");
    }
    if(typeNode->nodeType == NodeType::IDENTIFIER)
    {
        typeVar->types.push_back(*type);
        typeVar->names.push_back( std::move(*typeNode->data) );
        return;
    }

    AstNode* identifier = typeNode->children[0];  

    typeVar->types.push_back( *type + '#' + *typeNode->data);
    typeVar->names.push_back( std::move(*identifier->data) );
}
