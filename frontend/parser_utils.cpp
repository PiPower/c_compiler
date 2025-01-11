#include "parser_utils.hpp"
#include "parser.hpp"
#include "parser_internal.hpp"
#include <stdarg.h>
#include <string.h>
#include <random>
#include <algorithm>
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
    case TokenType::AMPERSAND :
        return NodeType::AND;
    case TokenType::CARET :
        return NodeType::EXC_OR;
    case TokenType::DOUBLE_AMPERSAND :
        return NodeType::LOG_AND;
    case TokenType::DOUBLE_PIPE :
        return NodeType::LOG_OR;
    }

    fprintf(stdout, "unexpected token at line %d \n", token.line);
    exit(-1);
}

uint8_t getTypeGroup(ParserState *parser, const std::string* name)
{   

    if(name->find('*')!= string::npos )
    {
        return SPECIAL_GROUP;
    }

    SymbolType* symType = getSymbolType(parser, name);
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
std::string *copyStrongerType(ParserState* parser, const std::string* t1, const std::string* t2)
{
    if(*t1 == *t2)
    {
        return new string(*t1);
    }

    SymbolType* symType1 = getSymbolType(parser, t1);
    SymbolType* symType2 = getSymbolType(parser, t2);
    if(symType1->affiliation > symType2->affiliation)
    {
        return new string(*t1);
    }

    return new string(*t2);
}

std::string* resolveImpConv(ParserState* parser, 
                                            const std::string* t1, 
                                            const std::string* t2, 
                                            uint8_t g1, 
                                            uint8_t g2)
{
    static const char* signedTypes[] = { "char", "short", "int", "long"};
    if( g1 == FLOAT_GROUP || g2 == FLOAT_GROUP)
    {
        const string* intType = g1 == FLOAT_GROUP ? t2 : t1;
        const string* floatType = g1 == FLOAT_GROUP ? t1 : t2;

        const uint8_t intGroup = g1 == FLOAT_GROUP? g2 : g1;

        SymbolType* signedSym = getSymbolType(parser, intType);
        SymbolType* floatSym = getSymbolType(parser, floatType);
        uint8_t intStrength = signedSym->affiliation - intGroup;

        bool below64Bit = intStrength <= (0x01 << 2);

        if( below64Bit && floatSym->affiliation == FLOAT32)
        {
            return new string("float");
        }
        return new string("double");
    }



    const string* signedType = g1 == SIGNED_INT_GROUP ? t1 : t2;
    const string* unsignedType = g1 == UNSIGNED_INT_GROUP ? t1 : t2;
    SymbolType* signedSym = getSymbolType(parser, signedType);
    SymbolType* unsignedSym = getSymbolType(parser, unsignedType);

    uint8_t signedStrength = signedSym->affiliation - SIGNED_INT_GROUP;
    uint8_t unsignedStrength = unsignedSym->affiliation - UNSIGNED_INT_GROUP;

    uint8_t stronger = max(signedStrength, unsignedStrength);
    uint8_t lo = 0;
    uint8_t hi = 3;
    uint8_t idx = clamp(stronger, lo, hi);
    return new string(signedTypes[idx]);
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
        // decoding type/qualifier info
        TypePair typeInfo = decodeType(root->type);
        var->varType = typeInfo.type;
        var->qualifiers = typeInfo.qualifiers;
        
        SET_SYMBOL(parser, *root->data, (Symbol*)var);
        setDefinedAttr(var);
        if(initializer || parser->symtab->scopeLevel == 0)
        {
            delete root->type;
            // copy properly decoded type info
            root->type = new string(*var->varType);
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
        TypePair pair = decodeType(root->type);
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
        if( *pair.type != *fn->retType)
        {
            triggerParserError(parser, 1, "Redeclared function %s has different return type than before\n", root->data->c_str());
        }
        delete pair.type;
        delete pair.qualifiers;
        if( args->children.size() != fn->argTypes.size())
        {
            triggerParserError(parser, 1, "Redeclared function %s has different arguments than before\n", root->data->c_str());
        }
        for(int i = 0; i < args->children.size(); i++ )
        {
            pair =  decodeType(args->children[i]->type);
            if(*(pair.type) != *(fn->argTypes[i]) )
            {
                delete pair.type;
                delete pair.qualifiers;
                triggerParserError(parser, 1, "Redeclared function %s has different arguments than before\n", root->data->c_str());
            }
            delete pair.type;
            delete pair.qualifiers;
        }
    }
    else
    {
        fn = new SymbolFunction;
        fn->attributes = 0;
        fn->fnStackSize = 0;
        fn->symClass = SymbolClass::FUNCTION;
        TypePair pair = decodeType(root->type);
        fn->retType = pair.type;
        fn->qualifiers = pair.qualifiers;
        SET_SYMBOL(parser, *root->data, (Symbol*)fn);
        *root->type = *fn->retType;
        if(root->nodeType == NodeType::FUNCTION_DEF)
        {
            setDefinedAttr(fn);
        }

        for( AstNode* param : args->children)
        {
            pair = decodeType(param->type);
            fn->argTypes.push_back(pair.type);
            fn->argQualifiers.push_back(pair.qualifiers);
            
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
        symType->typeSize = 0;
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
                            AstNode *paramNode)
{
    if(paramNode->nodeType != NodeType::IDENTIFIER && 
        paramNode->nodeType != NodeType::POINTER)
    {
        triggerParserError(parser, 1, "Incorrect declaration of name of the type\n");
    }

    if(paramNode->nodeType == NodeType::POINTER)
    {
        paramNode->children[0]->type = paramNode->type;
        paramNode->type = nullptr;
        *paramNode->children[0]->type += *paramNode->data;
        paramNode = paramNode->children[0];
    }

    TypePair pair = decodeType(paramNode->type);
    typeVar->types.push_back(std::move(*pair.type));
    typeVar->qualifiers.push_back(std::move(*pair.qualifiers));
    typeVar->names.push_back( std::move(*paramNode->data) );

    delete pair.type;
    delete pair.qualifiers;
    return;
}

SymbolType* getSymbolType(ParserState *parser, const std::string* name)
{
    Symbol* sym = GET_SYMBOL(parser, *name);
    if(!sym)
    {
        triggerParserError(parser, 1, "type named \"%s\" does not exist\n", name->c_str());
    }
    if(sym->symClass != SymbolClass::TYPE)
    {
        triggerParserError(parser, 1, "Implementation error: \"%s\" is not a typename\n", name->c_str());
    }
    return (SymbolType*)sym;
}

uint16_t getTypeAffiliation(ParserState *parser, std::string* name)
{
    if(name->find('*') != string::npos)
    {
        return POINTER_GR;
    }
    SymbolType* symType = getSymbolType(parser, name);
    return symType->affiliation;
}

//decode encoded type into (type, qualifier)
TypePair decodeType(const std::string *encodedType)
{
    TypePair pair;
    pair.type = new string();
    pair.qualifiers = new string();
    int i;
    for( i=0; i < encodedType->length(); i++)
    {
        if((*encodedType)[i] == '|')
        {
            i++;
            break;
        }
        *pair.type += (*encodedType)[i];
    }
    *pair.qualifiers += (*encodedType)[i++];
    while (i < encodedType->length())
    {
        *pair.type += '*';
        i++;
        *pair.qualifiers += (*encodedType)[i];
        i++;
    }

    return pair;
}

AstNode* unwindReturnType(ParserState *parser, AstNode *retPtr)
{

    AstNode* fn = retPtr->children[0];
    fn->type = retPtr->type;
    retPtr->type = nullptr;
    *fn->type += *retPtr->data;

    FREE_NODE(parser, retPtr);
    return fn;
}
