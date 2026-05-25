#include "Parser.hpp"
#include <cassert>
#include <sys/mman.h> 
#include <string.h>
#include <stdarg.h>
#include <future>
#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);


#define CPU_PAGE 4096
#define PAGE_COUNT 50
typedef Ast::Node* (Parser::*ParseFn)();

template<size_t Count>
static Ast::NodeType ResolveNodeType(
    const TokenType::Type& type,
    const std::array<TokenType::Type, Count>& operators,
    const std::array<Ast::NodeType, Count>& nodeOps )
{
    for(size_t i = 0; i < Count; i++)
    {
        if(type == operators[i])
        {
            return nodeOps[i];
        }
    }
    return Ast::none;
}

template<size_t Count>
static Ast::Node* ParseLoop(
    Parser* p, 
    ParseFn pFn, 
    const std::array<TokenType::Type, Count>& operators,
    const std::array<Ast::NodeType, Count>& nodeOps)
{
    Ast::Node* expr = (*p.*pFn)();
    Token token = p->GetCurrToken();

    while (IsTokenOneFromArray(&token, operators))
    {
        p->ConsumeToken();

        Ast::Node* node =  p->AllocateAstNodes();
        node->type = ResolveNodeType(token.type, operators, nodeOps);
        node->lChild = expr;
        node->rChild = (*p.*pFn)();
        node->token = token;
        expr = node;

        token =  p->GetCurrToken();
    }
    
    return expr;
}

template<size_t Count>
static Ast::Node* SpecifierParseLoop(
    Parser* p, 
    const std::array<TokenType::Type, Count>& specOperators,
    Ast::NodeType specType)
{
    Token token = p->GetCurrToken();

    if(!IsTokenOneFromArray(&token, specOperators))
    {
        return nullptr;
    }

    Ast::Node* specifier = p->AllocateAstNodes();
    specifier->type = specType;
    specifier->token = token;
    Ast::Node* bottomChild = specifier;

    p->ConsumeToken();
    token = p->GetCurrToken();
    while (IsTokenOneFromArray(&token, specOperators))
    {
        Ast::Node* node = p->AllocateAstNodes();
        *node = {};
        node->type = specType;
        node->token = token;
        bottomChild->lChild = node;
        bottomChild = node;

        p->ConsumeToken();
        token =  p->GetCurrToken();
    }
    
    return specifier;
}

Parser::Parser(FILE_ID mainFileId, SemanticAnalyzer* analyzer, FileManager* manager, const CompilationOpts* opts)
:
analyzer(analyzer), manager(manager), PP(mainFileId, manager, opts), 
opts(opts), pState({}), logger(manager, "Parser")
{
    assert(opts != nullptr);
    AddNodePage(); 
}

Ast::Node* Parser::Parse()
{
start_parsing:
    Token token = GetCurrToken();
    while (token.type == TokenType::new_line)
    {
        ConsumeToken();
        token = GetCurrToken();
    }
    if(token.location.id.id == 3 && token.location.line == 205)
    {
        int x = 2;
    }
    // when parser recives eof token it marks the end of translation unit
    if(token.type == TokenType::eof)
    {
        return nullptr;
    }
    // Check if constant expression is pending
    if(PP.stages.ConstantExpr > 0)
    {
        Ast::Node* expr = ParseConstantExpr();
        PP.ExecuteConstantExpr(expr);
        goto start_parsing;
    }   

    Ast::Node* declaration = ParseDeclaration(false);
    token = GetCurrToken();
    if(!IsTokenOneOf(&token, TokenType::semicolon, TokenType::l_brace))
    {
        IssueWarning(&token, "'declaration-specifiers declarator declaration compound-statement' is not supported");
    }
    if(token.type == TokenType::semicolon)
    {
        ConsumeExpectedToken(TokenType::semicolon);
        return declaration;
    }

    Ast::Node* functionDef = AllocateAstNodes();
    functionDef->type = Ast::function_def;
    functionDef->token = token;
    functionDef->lChild = declaration;
    functionDef->rChild = ParseCompoundStatement();

    return functionDef;
}

Token Parser::GetCurrToken()
{
    // this is temporary solution to skip kw__attribute__
    // to be resolved later
    Token token = GetCurrTokenInternal();
    while (IsTokenOneOf(&token, TokenType::kw__attribute__, TokenType::kw__asm__, TokenType::kw__extension__))
    {

        ConsumeToken();
        if(token.type == TokenType::kw__extension__)
        {
            token = GetCurrTokenInternal();
            continue;
        }
        ConsumeExpectedToken(TokenType::l_parentheses);
        int nest = 1;
        while (true)
        {
            token = GetCurrTokenInternal();
            ConsumeToken();
            if(token.type == TokenType::l_parentheses)
            {
                nest++;
            }
            else if(token.type == TokenType::r_parentheses)
            {
                nest--;
                if(nest == 0)
                {
                    break;
                }
            }
        }
    
    }
    
    return token;
}

Token Parser::GetCurrTokenInternal()
{
    get_token:
    if(tokenQueue.empty())
    {
        Token token;
        PP.Peek(&token);
        tokenQueue.push_back(token);
    }
    // if not parsing constant expression new-line does not serve as end of expression token
    if(pState.parsingConstantExpr == 0 && tokenQueue.front().type == TokenType::new_line )
    {
        tokenQueue.pop_front();
        goto get_token;
    }
    if(pState.parsingConstantExpr == 1 && tokenQueue.front().type == TokenType::line_splice )
    {
        tokenQueue.pop_front();
        goto get_token;
    }

    currentLocToken = tokenQueue.front();
    return tokenQueue.front();
}

void Parser::PutBackAtFront(Token token)
{
    tokenQueue.push_front(token);
}

void Parser::ConsumeToken()
{
    if(!tokenQueue.empty())
    {
        tokenQueue.pop_front();
    }
}

std::string_view Parser::GetViewForToken(const Token &token)
{
    FILE_STATE state;
    if(manager->GetFileState(&token.location.id, &state) != 0)
    {
        printf("Parser critical error: Requested file does not exit\n");
        exit(-1);
    }

    // removes \" from both start and end 
    uint8_t offset = token.type == TokenType::string_literal ? 1 : 0;
    std::string_view tokenView(state.fileData + token.location.offset + offset,
                                token.location.len - offset);
    return tokenView;
}

void Parser::ConsumeExpectedToken(TokenType::Type type)
{
    Token token = GetCurrToken();
    ConsumeToken();
    if(token.type != type)
    {
        IssueWarning(&token, 
        "Given token is [%s] but expected is [%s]", 
        TokenType::tokenStr(token.type), TokenType::tokenStr(type));
        exit(-1);
    }
    return;
}

Ast::Node *Parser::AllocateAstNodes(uint16_t count)
{
    size_t memorySize = count * sizeof(Ast::Node);
    if( memorySize >=  PAGE_COUNT * CPU_PAGE)
    {
        printf("Requested node array is too large\n");
        exit(-1);
    }
    
    if(memorySize + nodeBuffer.offsetIntoPage >= PAGE_COUNT * CPU_PAGE)
    {
        AddNodePage();
    }

    const char* dataBase = nodeBuffer.pages[nodeBuffer.currentPage] + nodeBuffer.offsetIntoPage;
    nodeBuffer.offsetIntoPage+= memorySize;
    // start lifetime of Ast::Node objects
    Ast::Node* nodes = new ((void*)dataBase)Ast::Node[count];
    memset(nodes, 0, memorySize);
    return nodes;
}

void Parser::AddNodePage()
{
    void* mmapRet = mmap(nullptr, PAGE_COUNT * CPU_PAGE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(mmapRet == MAP_FAILED )
    {
        printf("Error Message: %s \n", strerror(errno));
        exit(-1);
    }
    
    char* filePage = (char*)mmapRet;
    nodeBuffer.pages.push_back(filePage);
    nodeBuffer.offsetIntoPage = 0;
    nodeBuffer.currentPage = nodeBuffer.pages.size() - 1;
}

Ast::Node *Parser::GlueNodes(Ast::Node *l, Ast::Node *parent)
{
    Ast::Node* glueNode = AllocateAstNodes();
    glueNode->type = Ast::NodeType::glue_list;
    glueNode->lChild = l;
    parent->rChild = glueNode;
    return glueNode;
}

bool Parser::IsAssignment(Token *token)
{
    return IsAssignment(token->type);
}

bool Parser::IsAssignment(TokenType::Type type)
{
    return  TokenType::equal <= type && type <= TokenType::pipe_equal;
}

Ast::Node *Parser::ParseConstantExpr()
{
    pState.parsingConstantExpr = 1;
    Ast::Node* node = ConditionalExpression();
    pState.parsingConstantExpr = 0;
    return node;
}

Ast::Node* Parser::ParseExpression()
{
    Ast::Node* expr = AllocateAstNodes();
    expr->type = pState.parsingConstantExpr? Ast::constant_expression : Ast::expression;
    Ast::Node* topLevel = expr;
    while (true)
    {
        Ast::Node* asmExpr = AssignmentExpression();
        topLevel = GlueNodes(asmExpr, topLevel);
        if(GetCurrToken().type != TokenType::comma)
        {
            break;
        }
        ConsumeExpectedToken(TokenType::comma);
    }
    
    return expr;
}

Ast::Node* Parser::PrimaryExpression()
{
    static constexpr const char* expectedStrs[] = {
        tokenStr(TokenType::identifier), tokenStr(TokenType::numeric_constant),
        tokenStr(TokenType::string_literal), tokenStr(TokenType::l_string_literal),
        tokenStr(TokenType::character_literal), tokenStr(TokenType::l_parentheses)};

    Token token = GetCurrToken();
    ConsumeToken();
    Ast::Node* node;
    if(token.type != TokenType::l_parentheses)
    {
        node = AllocateAstNodes(1);
        node->token = token;
    }
    switch (token.type)
    {
    case TokenType::identifier:       
        node->type = Ast::identifier; break;
    case TokenType::numeric_constant: 
        node->type = Ast::constant; break;
    case TokenType::character_literal: 
        node->type = Ast::character; break;
    case TokenType::string_literal:
    case TokenType::l_string_literal: 
        if(pState.parsingConstantExpr == 1)
        {
            IssueWarning(&token, 
                "String literal is not allowed in constant expression");
            exit(-1);
        }
        node->type = Ast::string_literal;
        break;
    case TokenType::l_parentheses:
        node = ParseExpression();
        ConsumeExpectedToken(TokenType::r_parentheses);
        break;
    default:
        IssueWarning(&token, " Given token is [%s] but only [%s], [%s], [%s], [%s], [%s] or [%s] can be used in primary expression",
            tokenStr(token.type), expectedStrs[0], expectedStrs[1], expectedStrs[2], expectedStrs[3], expectedStrs[4], expectedStrs[5]);
        exit(-1);
        break;
    }
    return node;
}

Ast::Node *Parser::ParseIdentifier()
{
    Token token = GetCurrToken();
    ConsumeExpectedToken(TokenType::identifier);
    Ast::Node* node = AllocateAstNodes();
    node->token = token;
    node->type = Ast::NodeType::identifier;
    return node;
}

Ast::Node *Parser::ArgumentExprList()
{   
    Ast::Node* argExpr = AllocateAstNodes();
    argExpr->type = Ast::args_expr_list;
    argExpr->token = currentLocToken;

    Ast::Node* currTop = argExpr;
    while (true)
    {
        Ast::Node* asmExpr = AssignmentExpression();
        currTop = GlueNodes(asmExpr, currTop);

        if(GetCurrToken().type != TokenType::comma)
        {
            break;
        }
        ConsumeExpectedToken(TokenType::comma);
    }
    
    return argExpr;
}

Ast::Node *Parser::ParseCompoundStatement()
{

    Ast::Node* CompoundStatement = AllocateAstNodes();
    CompoundStatement->token = GetCurrToken();
    CompoundStatement->type = Ast::compound_statement;

    ConsumeExpectedToken(TokenType::l_brace);

    Ast::Node* currNode = CompoundStatement;
    while (true)
    {
        Ast::Node* blockItem = ParseDeclaration(true);
        if(!blockItem)
        {
            blockItem = ParseStatement();
        }
        if(blockItem)
        {
            currNode = GlueNodes(blockItem, currNode);
        }
        else
        {
            break;
        }

    }
    
    ConsumeExpectedToken(TokenType::r_brace);
    return CompoundStatement;
}

Ast::Node *Parser::ParseStatement()
{
    Token token = GetCurrToken();
    switch (token.type)
    {
    case TokenType::r_brace:     return nullptr;
    case TokenType::l_brace:     return ParseCompoundStatement();
    // labeled statement
    case TokenType::kw_case:     return Case();
    case TokenType::kw_default:  return Default();
    // selection statements
    case TokenType::kw_if:       return If();
    case TokenType::kw_switch:   return Switch();
    // Iteration statements
    case TokenType::kw_while:    return WhileLoop();
    case TokenType::kw_do:       return DoWhileLoop();
    case TokenType::kw_for:      return ForLoop();
    // Jump statements
    case TokenType::kw_goto:     return Goto();
    case TokenType::kw_continue: return Continue();
    case TokenType::kw_break:    return Break();
    case TokenType::kw_return:   return ReturnStatement();
    default:                     break;            
    }
    ConsumeToken();
    if(token.type == TokenType::identifier &&
       GetCurrToken().type == TokenType::colon)
    {
        PutBackAtFront(token);
        return Label();
    }
    else
    {
        PutBackAtFront(token);
    }

    Ast::Node* expr;
    if(token.type != TokenType::semicolon)
    {
        expr = ParseExpression();
    }
    else
    {
        expr = AllocateAstNodes();
        expr->token = token;
        expr->type = Ast::none;
    }
    ConsumeExpectedToken(TokenType::semicolon);
    return expr;
}

Ast::Node * Parser::ReturnStatement()
{
    Ast::Node* retStatement = AllocateAstNodes();
    retStatement->type = Ast::st_return;
    retStatement->token = GetCurrToken();
    ConsumeExpectedToken(TokenType::kw_return);

    if(GetCurrToken().type != TokenType::semicolon)
    {
        retStatement->lChild = ParseExpression();
    }
    ConsumeExpectedToken(TokenType::semicolon);

    return retStatement;
}

Ast::Node *Parser::ForLoop()
{
    Ast::Node* forLoop = AllocateAstNodes();
    forLoop->type = Ast::st_for_loop;
    forLoop->token = GetCurrToken();
    // for loop is stored as sequence
    // declaration/expr/None, expr/None, expr/None, statement
    forLoop->lChild = AllocateAstNodes(4);
    ConsumeExpectedToken(TokenType::kw_for);
    ConsumeExpectedToken(TokenType::l_parentheses);
    Ast::Node* nodeArray[4] = {nullptr, nullptr, nullptr, nullptr};


    if(GetCurrToken().type != TokenType::semicolon)
    {
        nodeArray[0] = ParseDeclaration(false);
        if(!nodeArray[0])
        {
            nodeArray[0] = ParseExpression();
        }
    }
    ConsumeExpectedToken(TokenType::semicolon);

    if(GetCurrToken().type != TokenType::semicolon)
    {
        nodeArray[1] = ParseExpression();
    }
    ConsumeExpectedToken(TokenType::semicolon);

    if(GetCurrToken().type != TokenType::r_parentheses)
    {
        nodeArray[2] = ParseExpression();
    }
    ConsumeExpectedToken(TokenType::r_parentheses);
    nodeArray[3] = ParseStatement();

    for(int i =0; i < 4; i++)
    {
        forLoop->lChild[i] = nodeArray[i] ? *nodeArray[i] : Ast::Node{};
    }
    return forLoop;
}

Ast::Node *Parser::WhileLoop()
{
    Ast::Node* nodeWhile = AllocateAstNodes();
    nodeWhile->type = Ast::st_while_loop;
    nodeWhile->token = GetCurrToken();
    ConsumeToken();
    ConsumeExpectedToken(TokenType::l_parentheses);
    nodeWhile->lChild = ParseExpression();
    ConsumeExpectedToken(TokenType::r_parentheses);
    nodeWhile->rChild = ParseStatement();

    return nodeWhile;
}

Ast::Node *Parser::DoWhileLoop()
{
    Ast::Node* nodeDoWhile = AllocateAstNodes();
    nodeDoWhile->type = Ast::st_do_while_loop;
    nodeDoWhile->token = GetCurrToken();
    ConsumeToken();
    nodeDoWhile->lChild = ParseStatement();
    ConsumeExpectedToken(TokenType::kw_while);
    ConsumeExpectedToken(TokenType::l_parentheses);
    nodeDoWhile->rChild = ParseExpression();
    ConsumeExpectedToken(TokenType::r_parentheses);

    return nodeDoWhile;
}

Ast::Node *Parser::If()
{
    Ast::Node* ifNode = AllocateAstNodes(4);
    ifNode->token = GetCurrToken();
    ifNode->type = Ast::st_if;
    ifNode->rChild = ifNode + 1;
    ConsumeToken();
    // controllable leaks, memory will be freed later by allocator
    ConsumeExpectedToken(TokenType::l_parentheses);
    ifNode->rChild[0] = *ParseExpression();
    ConsumeExpectedToken(TokenType::r_parentheses);
    ifNode->rChild[1] = *ParseStatement();
    if(GetCurrToken().type == TokenType::kw_else)
    {
        ConsumeToken();
        ifNode->rChild[2] = *ParseStatement();
    }
    return ifNode;
}

Ast::Node *Parser::Switch()
{
    Ast::Node* switchNode = AllocateAstNodes();
    switchNode->type = Ast::st_switch;
    switchNode->token = GetCurrToken();
    ConsumeToken();
    ConsumeExpectedToken(TokenType::l_parentheses);
    switchNode->lChild = ParseExpression();
    ConsumeExpectedToken(TokenType::r_parentheses);
    switchNode->rChild = ParseStatement();

    return switchNode;
}

Ast::Node *Parser::Case()
{
    Ast::Node* caseNode = AllocateAstNodes();
    caseNode->type = Ast::st_case;
    caseNode->token = GetCurrToken();
    ConsumeToken();
    caseNode->lChild = ParseConstantExpr();
    ConsumeExpectedToken(TokenType::colon);
    caseNode->rChild = ParseStatement();
    return caseNode;
}

Ast::Node *Parser::Break()
{
    Ast::Node* nodeBreak = AllocateAstNodes();
    nodeBreak->type = Ast::st_break;
    nodeBreak->token = GetCurrToken();
    ConsumeToken();
    ConsumeExpectedToken(TokenType::semicolon);
    return nodeBreak;
}

Ast::Node *Parser::Default()
{
    Ast::Node* nodeDef = AllocateAstNodes();
    nodeDef->type = Ast::st_default;
    nodeDef->token = GetCurrToken();
    ConsumeToken();
    ConsumeExpectedToken(TokenType::colon);
    nodeDef->lChild = ParseStatement();

    return nodeDef;
}

Ast::Node *Parser::Goto()
{
    ConsumeToken();
    Ast::Node* nodeGoto = AllocateAstNodes();
    nodeGoto->type = Ast::st_goto;
    nodeGoto->token = GetCurrToken();
    ConsumeExpectedToken(TokenType::identifier);

    return nodeGoto;
}

Ast::Node *Parser::Continue()
{
    Ast::Node* nodeCont = AllocateAstNodes();
    nodeCont->type = Ast::st_continue;
    nodeCont->token = GetCurrToken();
    ConsumeToken();
    ConsumeExpectedToken(TokenType::semicolon);

    return nodeCont;
}

Ast::Node *Parser::Label()
{
    Ast::Node* nodeLabel = AllocateAstNodes();
    nodeLabel->type = Ast::st_label;
    nodeLabel->token = GetCurrToken();
    ConsumeToken();
    ConsumeExpectedToken(TokenType::colon);
    nodeLabel->lChild = ParseStatement();

    return nodeLabel;
}

Ast::Node *Parser::ParseDeclaration(bool consumeSemicolon)
{
    Ast::Node* declSpec = ParseDeclSpec();
    if(!declSpec)
    {
        return nullptr;
    }
    Ast::Node* initDeclarationList = ParseInitDeclList();
    
    if(declSpec && consumeSemicolon)
    {
        ConsumeExpectedToken(TokenType::semicolon);
    }

    Ast::Node* declaration = AllocateAstNodes();
    declaration->type = Ast::declaration;
    declaration->lChild = declSpec;
    declaration->rChild = initDeclarationList;

    return declaration;
}

Ast::Node *Parser::ParseInitializer()
{
    Token token = GetCurrToken();
    if(token.type == TokenType::l_brace)
    {
        ConsumeExpectedToken(TokenType::l_brace);

        Ast::Node* initializerList = InitializerList();
        initializerList->token = token;

        if(GetCurrToken().type == TokenType::comma)
        {
            ConsumeExpectedToken(TokenType::comma);
        }
        ConsumeExpectedToken(TokenType::r_brace);

        return initializerList;
    }
    return AssignmentExpression();
}

Ast::Node *Parser::ParseInitDeclList()
{
    Ast::Node* declarator = ParseDeclarator();
    if(!declarator)
    {
        return nullptr;
    }

    Ast::Node* initDeclList = AllocateAstNodes();
    initDeclList->type = Ast::init_decl_list;
    Ast::Node* bottomChild = initDeclList;
    do
    {
        Ast::Node* initDeclarator = AllocateAstNodes();
        initDeclarator->type = Ast::init_declarator;
        initDeclarator->rChild = declarator;
        Token token = GetCurrToken();
        if(token.type == TokenType::equal)
        {
            ConsumeToken();
            initDeclarator->lChild = ParseInitializer();
        }

        bottomChild = GlueNodes(initDeclarator, bottomChild);

        if(GetCurrToken().type != TokenType::comma)
        {
            break;
        }
        ConsumeExpectedToken(TokenType::comma);
    } while ((declarator = ParseDeclarator()));
    
    return initDeclList;
}

Ast::Node *Parser::ParseDeclarator()
{
    Ast::Node* ptrExpr = ParsePointer();
    Ast::Node* declExpr = ParseDirectDeclarator();
    if(declExpr)
    {
        Ast::Node* declarator = AllocateAstNodes();
        declarator->type = Ast::NodeType::declarator;
        declarator->token = currentLocToken;
        declarator->lChild = declExpr;
        declarator->rChild = ptrExpr;
        return declarator;
    }

    if(ptrExpr && !declExpr)
    {
        IssueWarning(&ptrExpr->token, "It is not allowed to skip direct declarator part in declarator");
        exit(-1);
    }

    return nullptr;
}
Ast::Node *Parser::ParsePointer()
{
    Token token = GetCurrToken();
    if(token.type != TokenType::star)
    {
        return nullptr;
    }
    ConsumeToken();

    Ast::Node* ptr = AllocateAstNodes();
    ptr->lChild = TypeQualifierList();
    ptr->type = Ast::pointer;
    ptr->token = token;
    Ast::Node* bottom = ptr;
    
    token = GetCurrToken();
    while (token.type == TokenType::star)
    {   
        ConsumeToken();
        Ast::Node* subPtr = AllocateAstNodes();
        subPtr->type = Ast::pointer;
        subPtr->lChild = TypeQualifierList();
        subPtr->token = token;
        bottom->rChild = subPtr;
        bottom = subPtr;
        
        token = GetCurrToken();
    }
    return ptr;
}
Ast::Node *Parser::ParseFunctionCallArgs()
{
    bool isIdentifierList = false;
    // check if it is identifier list
    Token token = GetCurrToken();
    if(token.type == TokenType::identifier)
    {
        ConsumeToken();
        Token isComma = GetCurrToken();
        if(isComma.type == TokenType::comma)
        {
            isIdentifierList = true;
        }
        PutBackAtFront(token);
    }

    if(isIdentifierList)
    {
        Ast::Node* identifierList = AllocateAstNodes();
        identifierList->type = Ast::identifier_list;

        Ast::Node* bottomNode = identifierList;
        Token token = GetCurrToken();
        while (token.type == TokenType::identifier)
        {
            ConsumeToken();
            Ast::Node *identifier = AllocateAstNodes();
            identifier->type = Ast::identifier;
            identifier->token = token;
            bottomNode->lChild = identifier;
            bottomNode = identifier;

            token = GetCurrToken();
            if(token.type != TokenType::comma)
            {
                break;
            }
            ConsumeExpectedToken(TokenType::comma);
            token = GetCurrToken();
        }
        
        return identifierList;
    }
    return ParameterTypeList();

}
Ast::Node *Parser::ParseArrayArgs()
{
    bool requireAssmExpr = false, consumedStatic = false;
    Ast::Node* typeQuals = nullptr, *AssmExpr = nullptr;
    Token token = GetCurrToken();
    if(token.type == TokenType::kw_static)
    {
        requireAssmExpr = true;
        consumedStatic = true;
        ConsumeToken();
    }
    // parse type qualifier list
    typeQuals = TypeQualifierList();

    token = GetCurrToken();
    if(token.type == TokenType::kw_static)
    {
        requireAssmExpr = true;
        consumedStatic = true;
        ConsumeToken();
    }

    token = GetCurrToken();
    if(token.type == TokenType::star && !consumedStatic)
    {
        ConsumeToken();
        AssmExpr = AllocateAstNodes();
        AssmExpr->type = Ast::pointer;
        AssmExpr->token = token;
        goto merge_results;
    }
    // if right bracket then we have not expression  
    if(token.type != TokenType::r_bracket)
    {
        AssmExpr = AssignmentExpression();
    }
merge_results:

    if(requireAssmExpr)
    {
        IssueWarning(&AssmExpr->token, "Assignment expression is required in array expression");
        exit(-1);
    }

    Ast::Node* array = AllocateAstNodes();
    array->type = Ast::array_decl;
    array->lChild = typeQuals;
    array->rChild = AssmExpr;
    return array;
}
Ast::Node *Parser::ParseDirectDeclarator()
{
    Token token = GetCurrToken();
    Ast::Node* directDeclarator = nullptr;

    if(token.type == TokenType::l_parentheses)
    {
        ConsumeExpectedToken(TokenType::l_parentheses);
        Ast::Node* declarator = ParseDeclarator();
        if(!declarator)
        {
            PutBackAtFront(token);
            return nullptr;
        }
        directDeclarator = AllocateAstNodes();
        directDeclarator->type = Ast::direct_declarator;
        directDeclarator->token = token;
        directDeclarator->lChild = declarator;
        ConsumeExpectedToken(TokenType::r_parentheses);
    }
    else if(token.type == TokenType::identifier)
    {
        directDeclarator = AllocateAstNodes();
        directDeclarator->type = Ast::direct_declarator;
        directDeclarator->token = token;
        ConsumeToken();
    }
    else
    {
        return nullptr;
    }

    Ast::Node* bottomChild = directDeclarator;
    token = GetCurrToken();
    while (IsTokenOneOf(&token, TokenType::l_bracket, TokenType::l_parentheses))
    {
        if(token.type == TokenType::l_parentheses)
        {
            ConsumeExpectedToken(TokenType::l_parentheses);
            Ast::Node* array;
            if(GetCurrToken().type == TokenType::r_parentheses)
            {
                array = AllocateAstNodes();
                array->type = Ast::parameter_type_list ; // empty call is treated as identifier_list
            }
            else
            {
                array = ParseFunctionCallArgs();
            }

            array->token = token;
            ConsumeExpectedToken(TokenType::r_parentheses);

            bottomChild = GlueNodes(array, bottomChild);
        }

        if(token.type == TokenType::l_bracket)
        {
            ConsumeExpectedToken(TokenType::l_bracket);
            Ast::Node* array = ParseArrayArgs();
            array->token = token;
            ConsumeExpectedToken(TokenType::r_bracket);

            bottomChild = GlueNodes(array, bottomChild);
        }

        token = GetCurrToken();
    }
    
    return directDeclarator;
}
/*
    right child is used to chain different classes of specifiers,
    left child is internal to specific specifier
*/
Ast::Node *Parser::ParseDeclSpec()
{
    Ast::Node* declSpec = AllocateAstNodes();
    *declSpec = {};
    declSpec->type = Ast::NodeType::declaration_specifier;

    Ast::Node* bottomChild = declSpec;
    bool keepParsing = true;
    bool allowTypeSpec = true;
    while (keepParsing)
    {
        keepParsing = false;
        // right child of each top level decl ast is RESERVED for chaining
        if(Ast::Node* subDecl = DeclSpecSubtype(&allowTypeSpec))
        {
            keepParsing = true;
            bottomChild->rChild = subDecl;
            bottomChild = bottomChild->rChild;
        }
    }
    if(!declSpec->lChild && !declSpec->rChild)
    {
        // paged heap traces memory buffers so we have no leak
        return nullptr;
    }
    return declSpec;
}

Ast::Node *Parser::DeclSpecSubtype(bool* allowTypeSpec)
{
    if (Ast::Node* nodeOut = StorageSpec())
    {
        return nodeOut;
    }
    if (*allowTypeSpec)
    {
        if(Ast::Node* nodeOut = TypeSpecifier())
        {
            *allowTypeSpec = false;
            return nodeOut;
        }
    }
    if (Ast::Node* nodeOut = TypeQualifierList())
    {
        return nodeOut;
    }
    if (Ast::Node* nodeOut = FunctionSpec())
    {
        return nodeOut;
    }
    return nullptr;
}

Ast::Node *Parser::StorageSpec()
{
    std::array<TokenType::Type, 5> storageSpecifiers= {TokenType::kw_typedef, TokenType::kw_extern, 
                                                 TokenType::kw_static, TokenType::kw_auto, TokenType::kw_register};
    return SpecifierParseLoop(this, storageSpecifiers, Ast::NodeType::storage_specifier);
}

Ast::Node *Parser::InitializerList()
{
    Ast::Node* initializerList =  AllocateAstNodes();
    initializerList->type = Ast::initializer_list;

    Token token = GetCurrToken();
    Ast::Node* currParent = initializerList;
    while (token.type != TokenType::r_brace)
    {
        Ast::Node* initItem = AllocateAstNodes();
        initItem->type = Ast::init_item;
        initItem->token = token;
        initItem->rChild = DesignatorList();
        if(initItem->rChild)
        {
            ConsumeExpectedToken(TokenType::equal);
        }
        initItem->lChild = ParseInitializer();

        currParent = GlueNodes(initItem, currParent);
        token = GetCurrToken();
        if(token.type == TokenType::comma)
        {
            ConsumeExpectedToken(TokenType::comma);
            token = GetCurrToken();
        }
    }   
    
    return initializerList;
}

Ast::Node *Parser::TypeSpecifier()
{
    constexpr auto storageSpecifiers = std::to_array<TokenType::Type>({TokenType::kw_void, TokenType::kw_char, 
        TokenType::kw_short, TokenType::kw_int, TokenType::kw_long, TokenType::kw_float,
        TokenType::kw_double, TokenType::kw_signed, TokenType::kw_unsigned, TokenType::kw_bool,
        TokenType::kw_complex, TokenType::kw_imaginary,  TokenType::kw__builtin_va_list});

    Ast::Node* simpleSpecifiers = SpecifierParseLoop(this, storageSpecifiers, Ast::NodeType::type_specifier);
    // it is not allowed to chain simple specifier with complex ones
    // to make sema easier just return here
    if(simpleSpecifiers)
    {
        return simpleSpecifiers;
    }

    Token token = GetCurrToken();
    if(IsTokenOneOf(&token, TokenType::kw_struct, TokenType::kw_union))
    {
        return StructOrUnionSpec();
    }

    if(IsTokenOneOf(&token, TokenType::kw_enum))
    {
        return EnumSpec();
    }
    
    if(IsTokenOneOf(&token, TokenType::identifier))
    {
        if(analyzer->NamesAType(GetViewForToken(token)))
        {
            ConsumeToken();
            Ast::Node* type = AllocateAstNodes();
            type->type = Ast::type_specifier;
            type->token = token;
            return type;
        }
        return nullptr;
    }
    
    return nullptr;
}

Ast::Node *Parser::TypeName()
{
    Ast::Node* typeName = AllocateAstNodes();
    typeName->type = Ast::type_name;

    //specifier-qualifier-list
    Ast::Node* bottomChild = typeName;
    bool keepParsing = true;
    bool allowTypeSpec = true;
    while (keepParsing)
    {
        keepParsing = false;
        // right child of each top level decl ast is RESERVED for chaining
        if(Ast::Node* subDecl = DeclSpecSubtype(&allowTypeSpec))
        {
            if(subDecl->type == Ast::storage_specifier || 
                subDecl->type == Ast::function_specifier)
            {
                printf("'%s' is not allowed inside type-name\n", Ast::nodeStr(subDecl->type));
                exit(-1);
            }

            keepParsing = true;
            bottomChild->rChild = subDecl;
            bottomChild = bottomChild->rChild;
        }
    }    
    if(!typeName->rChild)
    {
        return nullptr;
    }
    typeName->lChild = AbstractDeclarator();
    return typeName;
}

Ast::Node *Parser::TypeQualifierList()
{
    std::array<TokenType::Type, 3> typeQuelifiers = {TokenType::kw_const,
         TokenType::kw_restrict, TokenType::kw_volatile};

   return SpecifierParseLoop(this, typeQuelifiers, Ast::NodeType::type_qualifier);
}

Ast::Node *Parser::FunctionSpec()
{
    constexpr std::array<TokenType::Type, 1> funcSpecifiers = {TokenType::kw_inline};
    return SpecifierParseLoop(this, funcSpecifiers, Ast::NodeType::function_specifier);
}

Ast::Node *Parser::StructOrUnionSpec()
{
    Token structOrUnion = GetCurrToken();
    ConsumeToken();

    // topLevelNode serves as a handle to ast for struct because right child MUST BE not used
    // right child is used as chain link node in general
    Ast::Node* topLevelNode = AllocateAstNodes();
    topLevelNode->type = Ast::NodeType::type_specifier;
    topLevelNode->token = structOrUnion;
    // glue node is used for stiching whole struct ast together
    Ast::Node* glueNode = AllocateAstNodes();
    glueNode->type = Ast::NodeType::glue_list;
    topLevelNode->lChild = glueNode;

    Token token = GetCurrToken();
    if(token.type == TokenType::identifier)
    {
        ConsumeToken();
        Ast::Node* identifier = AllocateAstNodes();
        identifier->type=Ast::NodeType::identifier;
        identifier->token = token;
        glueNode->lChild = identifier;

        token = GetCurrToken();
        if(token.type != TokenType::l_brace)
        {
            // we hit case "struct-or-union identifier"
            return topLevelNode;
        }
        
    }
    ConsumeExpectedToken(TokenType::l_brace);
    // declarations will be held as a linked list in the right subtree
    // right subtree is struct declaration list
    Ast::Node* bottomChild = glueNode;
    while (Ast::Node* structDecl = StructDeclaration())
    {
        bottomChild = GlueNodes(structDecl, bottomChild);

        token = GetCurrToken();
        if(token.type == TokenType::r_brace)
        {
            break;
        }
    }
    
    ConsumeExpectedToken(TokenType::r_brace);

    return topLevelNode;
}

Ast::Node *Parser::EnumSpec()
{
    Ast::Node* topLevelNode = AllocateAstNodes(2);
    topLevelNode->type = Ast::NodeType::type_specifier;
    topLevelNode->token = GetCurrToken();

    Ast::Node* enumNode = topLevelNode + 1;
    enumNode->type = Ast::enum_declaration;
    topLevelNode->lChild = enumNode;


    ConsumeExpectedToken(TokenType::kw_enum);
    Token token = GetCurrToken();
    if(token.type == TokenType::identifier)
    {
        enumNode->token = token;
        ConsumeToken();
        if(GetCurrToken().type != TokenType::l_brace)
        {
            // we hit case enum-specifier
            return topLevelNode;
        }

    }
    ConsumeExpectedToken(TokenType::l_brace);

    token = GetCurrToken();

    Ast::Node* currNode = enumNode;
    while (token.type == TokenType::identifier)
    {
        ConsumeToken();
        Ast::Node* enumerator = AllocateAstNodes();
        enumerator->token = token;
        enumerator->type = Ast::enumerator;
        if(GetCurrToken().type == TokenType::equal)
        {
            ConsumeToken();
            enumerator->lChild = ParseConstantExpr();
        }

        currNode->rChild = enumerator;
        currNode = enumerator;

        if(GetCurrToken().type == TokenType::comma)
        {
            ConsumeExpectedToken(TokenType::comma);
        }
        token = GetCurrToken();
    }
    ConsumeExpectedToken(TokenType::r_brace);

    return topLevelNode;
}

Ast::Node *Parser::StructDeclaration()
{
    Ast::Node* structDecl = AllocateAstNodes();
    structDecl->type = Ast::NodeType::struct_declaration;

    Ast::Node handleNode;
    Ast::Node* bottomChild = &handleNode;
    // parse qualifier specifier list
    bool keepParsing = true;
    while (keepParsing)
    {
        keepParsing = false;
        if(Ast::Node* qualifiers = TypeQualifierList())
        {
            keepParsing = true;
            bottomChild->rChild = qualifiers;
            bottomChild = qualifiers;
        }

        if(Ast::Node* typeSpec = TypeSpecifier())
        {
            keepParsing = true;
            bottomChild->rChild = typeSpec;
            bottomChild = typeSpec;
        }
    }
    structDecl->lChild = handleNode.rChild;
    // if left part of tree does not exist we have error
    if(structDecl->lChild  == nullptr)
    {
        IssueWarning(&structDecl->token, "Struct declaration does not contain specifier-qualifier list\n");
        exit(-1);
    }

    // parse struct declarator list
    // reset handle 
    bottomChild = structDecl;
    while (Ast::Node* declarator = StructDeclarator())
    {
        bottomChild = GlueNodes(declarator, bottomChild);
        if(GetCurrToken().type != TokenType::comma)
        {
            break;
        }
        ConsumeExpectedToken(TokenType::comma);
    }
    
    ConsumeExpectedToken(TokenType::semicolon);
    return structDecl;
}

Ast::Node *Parser::StructDeclarator()
{
    Ast::Node* declarator = nullptr;
    Ast::Node* constantExpr = nullptr;

    declarator = ParseDeclarator();
    Token token = GetCurrToken();
    if(token.type == TokenType::colon)
    {
        ConsumeToken();
        constantExpr = ParseConstantExpr();
        if(declarator != nullptr && constantExpr == nullptr)
        {
            IssueWarning(&token, "If struct declarator has 'identifier :' then it MUST have expression: 'constant-expr'");
            exit(-1);
        }
        Typed::Number bitCount = PP.ExecuteNode(constantExpr);
        constantExpr = AllocateAstNodes();
        constantExpr->type = Ast::constant_expr_res;
        static_assert(sizeof(Typed::Number) <= 2 * sizeof(Ast::Node*));
        memcpy(&constantExpr->lChild, &bitCount, sizeof(Typed::Number));
    }

    Ast::Node* structDecl = AllocateAstNodes();
    structDecl->type = Ast::struct_declarator;
    structDecl->lChild = declarator;
    structDecl->rChild = constantExpr;
    return structDecl;
}

Ast::Node *Parser::AbstractDeclarator()
{
    Ast::Node* ptrExpr = ParsePointer();
    Ast::Node* declExpr = ParseDirectAbstractDeclarator();

    Ast::Node* abstractDeclarator = AllocateAstNodes();
    abstractDeclarator->token = currentLocToken;
    abstractDeclarator->type = Ast::NodeType::abstact_declarator;
    abstractDeclarator->lChild = declExpr;
    abstractDeclarator->rChild = ptrExpr;
    return abstractDeclarator;
}

Ast::Node *Parser::ParameterDecl()
{
    // parsing: 
    // declaration-specifiers declarator
    // declaration-specifiers abstract-declarator_opt
    Ast::Node* generalDecl = AllocateAstNodes();
    Ast::Node* declSpec = ParseDeclSpec();
    Ast::Node* ptrExpr = ParsePointer();
    Ast::Node* declarator = ParseDirectDeclarator();
    if(!declarator)
    {
        declarator = ParseDirectAbstractDeclarator();
        /*if(!ptrExpr && !declarator)
        {
            //IssueWarning(nullptr, "Empty parameter declarator");
            return nullptr;
        }*/
        generalDecl->type = Ast::NodeType::abstact_declarator;
        generalDecl->rChild = ptrExpr;
        generalDecl->lChild = declarator;
    }
    else
    {
        generalDecl->type = Ast::NodeType::declarator;
        generalDecl->rChild = ptrExpr;
        generalDecl->lChild = declarator;
    }


    if(!declSpec )
    {
        IssueWarning(&currentLocToken, "Expected type specifier")
    }
    Ast::Node *parameterDecl = AllocateAstNodes();
    parameterDecl->type = Ast::parameter_decl;
    parameterDecl->lChild = declSpec;
    parameterDecl->rChild = generalDecl;

    return parameterDecl;
}

Ast::Node *Parser::ParseDirectAbstractDeclarator()
{
    Token token = GetCurrToken();
    if(!IsTokenOneOf(&token, TokenType::l_bracket, TokenType::l_parentheses))
    {
        return nullptr;
    }

    Ast::Node* directAbstractDeclarator = AllocateAstNodes();
    directAbstractDeclarator->type = Ast::direct_abstract_declarator;

    Ast::Node* bottomChild = directAbstractDeclarator;
    while(IsTokenOneOf(&token, TokenType::l_bracket, TokenType::l_parentheses))
    {
        ConsumeToken();
        if(token.type == TokenType::l_parentheses)
        {
            Ast::Node* abst;
            Token nextToken = GetCurrToken();
            if(IsTokenOneOf(&nextToken, TokenType::l_bracket, TokenType::l_parentheses, TokenType::star))
            {
                abst = AbstractDeclarator();
            }
            else
            {
                abst = ParameterTypeList();
            }
            ConsumeExpectedToken(TokenType::r_parentheses);

            bottomChild = GlueNodes(abst, bottomChild);
        }
        else
        {
            Ast::Node* array = AllocateAstNodes();
            array->token = token;
            array->type = Ast::array_decl;
            if(GetCurrToken().type != TokenType::r_bracket)
            {
                array->rChild = AssignmentExpression();
            }

            Token star = GetCurrToken();
            if(!array->rChild && star.type == TokenType::star)
            {
                array->type = Ast::var_len_array;
                ConsumeExpectedToken(TokenType::star);
            }
            ConsumeExpectedToken(TokenType::r_bracket);

            bottomChild = GlueNodes(array, bottomChild);
        }

        token = GetCurrToken();
    }

    return directAbstractDeclarator;
}

Ast::Node *Parser::ParameterTypeList()
{
    Ast::Node* parameterTypeList = AllocateAstNodes();
    parameterTypeList->type = Ast::parameter_type_list;
    Ast::Node* bottomChild = parameterTypeList;
    while (true)
    {
        Ast::Node *parameterDecl = ParameterDecl();

        bottomChild = GlueNodes(parameterDecl, bottomChild);

        Token token = GetCurrToken();
        if(token.type != TokenType::comma)
        {
            break;
        }
        ConsumeToken();

        token = GetCurrToken();
        if(token.type == TokenType::ellipsis)
        {
            ConsumeToken();
            Ast::Node *ellipsis = AllocateAstNodes();
            ellipsis->type = Ast::parameter_decl;
            ellipsis->token = token;
            bottomChild->rChild = ellipsis;
            break;
        }
    }
    
    return parameterTypeList;
}

Ast::Node *Parser::DesignatorList()
{
    Token token = GetCurrToken();
    if(!IsTokenOneOf(&token,  TokenType::dot, TokenType::l_bracket))
    {
        return nullptr;
    }

    Ast::Node* desList = AllocateAstNodes();
    desList->type = Ast::designator_list;
    desList->token = token;

    Ast::Node* currNode = desList;
    while (IsTokenOneOf(&token,  TokenType::dot, TokenType::l_bracket))
    {
        ConsumeToken();
        Ast::Node* designator = AllocateAstNodes();
        designator->token = token;
        designator->type = Ast::designator;
        if(token.type == TokenType::l_bracket)
        {
            designator->lChild = ParseConstantExpr();
            ConsumeExpectedToken(TokenType::r_bracket);
        }
        else
        {
            // token holds identifier value
            designator->token = GetCurrToken();
            ConsumeExpectedToken(TokenType::identifier);
        }

        currNode->rChild = designator;
        currNode = designator;

        token = GetCurrToken();
    }
    
    return desList;
}

Ast::Node* Parser::PostfixExpression()
{
    Token token = GetCurrToken();
    if(token.type == TokenType::l_brace)
    {
        return ParseInitializer();
    }

    Ast::Node* postfixExpr = PrimaryExpression();

    token = GetCurrToken();
    while (IsTokenOneOf(&token, TokenType::l_bracket, TokenType::l_parentheses, 
            TokenType::dot, TokenType::arrow, TokenType::plus_plus, TokenType::minus_minus))
    {
        ConsumeToken();
        Ast::Node* node = AllocateAstNodes();
        node->token = token;
        
        if(token.type == TokenType::l_bracket)
        {
            node->type = Ast::NodeType::array_access;
            node->lChild = postfixExpr;
            node->rChild = ParseExpression();
            ConsumeExpectedToken(TokenType::r_bracket);
        }
        else if(token.type == TokenType::l_parentheses)
        {
            node->type = Ast::NodeType::function_call;
            node->lChild = postfixExpr;
            if(GetCurrToken().type != TokenType::r_parentheses)
            {
                node->rChild = ArgumentExprList();
            }
            ConsumeExpectedToken(TokenType::r_parentheses);
        }
        else if(token.type == TokenType::dot)
        {
            node->type = Ast::NodeType::struct_access;
            node->lChild = postfixExpr;
            node->rChild = ParseIdentifier();
        }
        else if(token.type == TokenType::arrow)
        {
            node->type = Ast::NodeType::ptr_access;
            node->lChild = postfixExpr;
            node->rChild = ParseIdentifier();
        }
        else if(token.type == TokenType::plus_plus)
        {
            node->type = Ast::NodeType::op_post_inc;
            node->lChild = postfixExpr;
        }
        else
        {
            node->type = Ast::NodeType::op_post_dec;
            node->lChild = postfixExpr;
        }

        token = GetCurrToken();
        postfixExpr = node;

    }
    
    return postfixExpr;
}

Ast::Node* Parser::UnaryExpression()
{
    constexpr std::array<TokenType::Type, 7> tokTypes = 
        {TokenType::ampersand, TokenType::star, TokenType::plus, 
         TokenType::minus, TokenType::tilde, TokenType::bang};
    constexpr std::array<Ast::NodeType, 7> opTypes = 
        {Ast::get_addr, Ast::ptr_access, Ast::op_plus, 
         Ast::op_minus, Ast::op_complement, Ast::op_log_negate};

    Token token = GetCurrToken();
    if(token.type == TokenType::kw_sizeof)
    {
        ConsumeToken();
        token = GetCurrToken();

        Ast::Node* node =  AllocateAstNodes();
        node->token = token;
        node->type = Ast::op_sizeof;
        if(token.type == TokenType::l_parentheses)
        {
            ConsumeExpectedToken(TokenType::l_parentheses);
            node->lChild = TypeName();
            if(!node->lChild)
            {
                PutBackAtFront(token);
                goto unary_expr;
            }
            node->token = token;
            ConsumeExpectedToken(TokenType::r_parentheses);

        }
unary_expr:
        if(!node->lChild)
        {
            node->lChild = UnaryExpression();
        }
        return node;
    }

    Ast::Node* unary = nullptr;
    token = GetCurrToken();
    while (IsTokenOneOf(&token, TokenType::plus_plus, TokenType::minus_minus))
    {
        ConsumeToken();
        unary = AllocateAstNodes();
        unary->token = token;
        unary->rChild = unary;
        switch (token.type )
        {
        case TokenType::plus_plus: unary->type = Ast::NodeType::op_pre_inc; break;
        case TokenType::minus_minus: unary->type = Ast::NodeType::op_pre_dec; break;
        default:  IssueWarning(&token, "Unexpected token in unary expression");
        }

        token = GetCurrToken();
    }


    if(IsTokenOneFromArray(&token, tokTypes))
    {
        ConsumeToken();
        Ast::Node* node =  AllocateAstNodes();
        node->type = ResolveNodeType(token.type, tokTypes, opTypes);
        node->lChild = CastExpression();
        node->rChild = unary;
        node->token = token;
        return node;
    }
    
    Ast::Node* postfix = PostfixExpression();
    if(unary)
    {
        unary->rChild = postfix;
        return unary;
    }
    return postfix;
}

Ast::Node* Parser::CastExpression()
{
    Token token = GetCurrToken();
    Ast::Node* cast = nullptr;
    Ast::Node* bottomChild = nullptr;
    while (token.type == TokenType::l_parentheses)
    {
        ConsumeToken();
        Ast::Node* typeName = TypeName();
        if(!typeName)
        {
            PutBackAtFront(token);
            break;
        }
        ConsumeExpectedToken(TokenType::r_parentheses);
        token = GetCurrToken();

        if(!cast)
        {
            cast = AllocateAstNodes();
            cast->type = Ast::cast;
            cast->token = token;
            bottomChild = cast;
        }

        Ast::Node* glueNode = AllocateAstNodes();
        glueNode->type = Ast::glue_list;
        glueNode->lChild = typeName;
        bottomChild->rChild = glueNode;
        bottomChild = glueNode->rChild;
    }
    

    Ast::Node* unary = UnaryExpression();
    token = GetCurrToken();
    if(unary && IsAssignment(token.type))
    {
        if(cast)
        {
            IssueWarning(&cast->token, "assignment expression cannot follow  cast expr");
        }
        Ast::Node* asmExpr = AssignmentExpression(unary);
        return asmExpr;
    }

    if(cast)
    {
        cast->lChild = unary;
        return cast;
    }

    return unary;
}

Ast::Node* Parser::MultiplicativeExpression()
{
    constexpr std::array<TokenType::Type, 3> tokTypes = {TokenType::star, TokenType::slash, TokenType::percent};
    constexpr std::array<Ast::NodeType, 3> opTypes = {Ast::op_multiply, Ast::op_divide, Ast::op_divide_modulo};
    return ParseLoop(this, &Parser::CastExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::AdditiveExpression()
{
    constexpr std::array<TokenType::Type, 2> tokTypes = {TokenType::plus, TokenType::minus};
    constexpr std::array<Ast::NodeType, 2> opTypes = {Ast::op_add, Ast::op_subtract};
    return ParseLoop(this, &Parser::MultiplicativeExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::ShiftExpression()
{
    constexpr std::array<TokenType::Type, 2> tokTypes = {TokenType::l_shift, TokenType::r_shift};
    constexpr std::array<Ast::NodeType, 2> opTypes = {Ast::op_l_shift, Ast::op_r_shift};
    return ParseLoop(this, &Parser::AdditiveExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::RelationalExpression()
{
    constexpr std::array<TokenType::Type, 4> tokTypes = {
        TokenType::less, TokenType::greater, TokenType::less_equal, TokenType::greater_equal};
    constexpr std::array<Ast::NodeType, 4> opTypes = {
        Ast::op_less, Ast::op_greater, Ast::op_less_equal, Ast::op_greater_equal};
    return ParseLoop(this, &Parser::ShiftExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::EqualityExpression()
{
    constexpr std::array<TokenType::Type, 2> tokTypes = {TokenType::equal_equal, TokenType::bang_equal};
    constexpr std::array<Ast::NodeType, 2> opTypes = {Ast::op_equal, Ast::op_not_equal};
    return ParseLoop(this, &Parser::RelationalExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::AndExpression()
{
    constexpr std::array<TokenType::Type, 1> tokTypes = {TokenType::ampersand};
    constexpr std::array<Ast::NodeType,1> opTypes = {Ast::op_and};
    return ParseLoop(this, &Parser::EqualityExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::ExclusiveOrExpression()
{
    constexpr std::array<TokenType::Type, 1> tokTypes = {TokenType::caret};
    constexpr std::array<Ast::NodeType,1> opTypes = {Ast::op_exc_or};
    return ParseLoop(this, &Parser::AndExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::InclusiveOrExpression()
{
    constexpr std::array<TokenType::Type, 1> tokTypes = {TokenType::pipe};
    constexpr std::array<Ast::NodeType,1> opTypes = {Ast::op_inc_or};
    return ParseLoop(this, &Parser::ExclusiveOrExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::LogicalAndExpression()
{
    constexpr std::array<TokenType::Type, 1> tokTypes = {TokenType::double_ampersand};
    constexpr std::array<Ast::NodeType,1> opTypes = {Ast::op_log_and};
    return ParseLoop(this, &Parser::InclusiveOrExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::LogicalOrExpression()
{
    constexpr std::array<TokenType::Type, 1> tokTypes = {TokenType::double_pipe};
    constexpr std::array<Ast::NodeType,1> opTypes = {Ast::op_log_or};
    return ParseLoop(this, &Parser::LogicalAndExpression, tokTypes,  opTypes);
}

Ast::Node* Parser::ConditionalExpression()
{
    Ast::Node* condExpr = LogicalOrExpression();
    Token token = GetCurrToken();
    if(token.type == TokenType::question_mark)
    {
        ConsumeToken();
        Ast::Node* logicalExpr = AllocateAstNodes(2);
        Ast::Node* expr = logicalExpr + 1;
        *expr = *ParseExpression();
        ConsumeExpectedToken(TokenType::colon);
        // swap pointer so that logicalExpr takes over condExpr
        // then condExpr becomes ConditionalExpression node
        *logicalExpr = *condExpr;
        *condExpr = {};
        condExpr->type = Ast::cond_expression;
        condExpr->token = token;
        condExpr->lChild = logicalExpr; // left child is array of  [logicalExpr, expr]
        condExpr->rChild = ConditionalExpression(); 
    }
    return condExpr;
}

Ast::Node* Parser::AssignmentExpression(Ast::Node* unaryExpr)
{
    if(!unaryExpr)
    {
        return ConditionalExpression();
    }

    Token token = GetCurrToken();
    if(IsAssignment(token.type))
    {
        ConsumeToken();
        Ast::Node* asmExpr = AllocateAstNodes();
        asmExpr->token = token;
        asmExpr->type = Ast::assignment;
        asmExpr->lChild = unaryExpr;
        asmExpr->rChild = AssignmentExpression();

        return asmExpr;
    }

    return nullptr;
}

Ast::NodeType Parser::GetAssignmentType(TokenType::Type asmType)
{
    return Ast::NodeType();
}
