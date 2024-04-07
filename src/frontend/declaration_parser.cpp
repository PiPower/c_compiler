#include "../../include/frontend/parser.hpp"
#include "../../include/frontend/scanner.hpp"
using namespace std;
#define LOW_SPEC_INDEX ( (int) TokenType::RESTRICT )
#define HIGH_SPEC_INDEX ((int)TokenType::_COMPLEX)
static bool parsedFunctionBody = false;

static AstNode* parseDeclarator(Scanner& scanner);

static long int dataTypeToByteSize(DataType type)
{
    if(type != DataType::INT_64)
    {
        fprintf(stdout, "unsupported data type");
        exit(-1);
    }
    return 8;
}
static long int getBlockRuntimeByteSize(AstNode* node)
{
    if(node->nodeType != NodeType::BLOCK)
    {
        fprintf(stdout, "unsupported node type in runtime size calculation\n");
        exit(-1);
    }

    long int size = 0;
    for(AstNode* child : node->children)
    {
        if(child->nodeType != NodeType::DECLARATION)
        {
            continue;
        }
        size += dataTypeToByteSize(child->nodeDataType) * child->children.size();
    }
    return size;
}

static NodeDataType parseDeclarationSpecifier(Scanner& scanner)
{
    int currentTokenID = (int) scanner.getCurrentToken().type;
    vector<Token> declSpecifiers = {};
    while (currentTokenID >= LOW_SPEC_INDEX && currentTokenID <= HIGH_SPEC_INDEX)
    {
        declSpecifiers.push_back(scanner.popToken());
        currentTokenID = (int) scanner.getCurrentToken().type;
    }
    return transformToDataType(declSpecifiers);
}


static AstNode* parseParameterDeclaration(Scanner& scanner)
{
    NodeDataType type = parseDeclarationSpecifier(scanner);
    AstNode* decl = new AstNode{NodeType::DECLARATION, {parseDeclarator(scanner)}, type};
    return decl;
}

static AstNode* parseParameterList(Scanner& scanner)
{
    AstNode* params = new AstNode{NodeType::FUNCTION_PARAMS,  {}, NodeDataType::NONE};
    while (true)
    {
        if(scanner.match(TokenType::R_PARENTHESES))
        {
            break;
        }
        AstNode* arg = parseParameterDeclaration(scanner);
        params->children.push_back(arg);
        if(scanner.match(TokenType::R_PARENTHESES))
        {
            break;
        }
        scanner.consume(TokenType::COMMA);
    }


    return params;
}

// function-definition:
//      declaration-specifiers declarator compound-statement
// TODO: what is declaration-list_opt ???? 
static AstNode* parseDeclarator(Scanner& scanner)
{
    if(!scanner.currentTokenOneOf({TokenType::IDENTIFIER}))
    {
        fprintf(stdout, "Unexpected token at line %d", scanner.popToken().line);
        exit(-1);
    }
    Token token = scanner.popToken();
    
    AstNode* Identifier = new AstNode{NodeType::IDENTIFIER, {}, NodeDataType::NONE, new string()};
    *((string*)Identifier->context.arbitraryData) = *(string*)token.context.data;

    if(!scanner.match(TokenType::L_PARENTHESES))
    {
        return Identifier;
    }

    AstNode* functionParams = parseParameterList(scanner);
    AstNode* functionDecl = new AstNode{NodeType::FUNCTION_DECLARATION, {Identifier, functionParams}, NodeDataType::NONE};
    if(scanner.currentTokenOneOf({TokenType::SEMICOLON}))
    {
        return functionDecl;
    }
    functionDecl->nodeType = NodeType::FUNCTION_DEFINITION;
    functionDecl->children.push_back(parseCompoundStatement(scanner));
    parsedFunctionBody = true;
    functionDecl->context.int_64 = getBlockRuntimeByteSize( functionDecl->children[2]);
    return functionDecl;
}

static AstNode* parseInitializer(Scanner& scanner)
{
    return assignmentExpression(scanner);
}

static AstNode* parseInitDeclarator(Scanner& scanner)
{
    AstNode* declarator = parseDeclarator(scanner);
    AstNode* root;
    if(declarator->nodeType != NodeType::FUNCTION_DECLARATION &&
        declarator->nodeType != NodeType::FUNCTION_DEFINITION )
    {
        root = new AstNode{NodeType::DEFINITION_VARIABLE, {declarator}, NodeDataType::INFERED};
    }
    else
    {
        root = declarator;
    }

    if(!scanner.match(TokenType::EQUAL))
    {
        return root;
    }

    AstNode* initializer = parseInitializer(scanner);
    root->children.push_back(initializer);
    return root;
}

AstNode* parseDeclaration(Scanner& scanner)
{

    NodeDataType dataType = parseDeclarationSpecifier(scanner);
    AstNode* root_parent = new AstNode{NodeType::DECLARATION, {}, dataType};
    
    if(scanner.match(TokenType::SEMICOLON))
    {
        delete root_parent;
        return nullptr;
    }

    AstNode* init_declarator = parseInitDeclarator(scanner);

    root_parent->children.push_back(init_declarator);

    while (scanner.match(TokenType::COMMA))
    {
        root_parent->children.push_back(parseInitDeclarator(scanner));
    }
    if(!parsedFunctionBody)
    {
        scanner.consume(TokenType::SEMICOLON);
    }
    return root_parent;
}
