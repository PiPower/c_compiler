#include "Parser.hpp"
#include <cassert>
#include <sys/mman.h> 
#include <string.h>
#include <stdarg.h>
#define CPU_PAGE 4096
#define PAGE_COUNT 50
typedef Ast::Node* (Parser::*ParseFn)();

template <std::size_t Count, std::size_t... Indices>
static bool IsTokenOneFromArray(
    const Token* token,
    const std::array<TokenType::Type, Count>& types,
    const std::index_sequence<Indices...>&) 
{
    return IsTokenOneOf(token, types[Indices]...); 
}

template <std::size_t Count>
static bool IsTokenOneFromArray(
    const Token* token,
    const std::array<TokenType::Type, Count>& types) 
{
    return IsTokenOneFromArray(token, types,  std::make_index_sequence<Count>{}); 
}

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

Parser::Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
manager(manager), PP(mainFile, manager, opts), opts(opts), unaryHandle(nullptr), pState({})
{
    assert(opts != nullptr);
    AddNodePage();
}

void Parser::Parse()
{
    // temporary loop
    while (true)
    {
        /* code */
start_parsing:
        Token token = GetCurrToken();
        while (token.type == TokenType::new_line)
        {
            ConsumeToken();
            token = GetCurrToken();
        }

        
        if(PP.stages.If > 0)
        {
            Ast::Node* expr = ParseConstantExpr();
            PP.ExecuteConstantExpr(expr);
            goto start_parsing;
        }

    }


}

Token Parser::GetCurrToken()
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

void Parser::IssueWarning(const Token * token, const char *errMsg, ...)
{
    va_list args;
    va_start(args, errMsg);
    if(token) {IssueWarning(&token->location.id, &token->location, errMsg, args);}
    else{IssueWarning(nullptr, nullptr, errMsg, args);}
    va_end(args);

    return;
}

void Parser::IssueWarning(const FILE_ID *fileId, const SourceLocation *loc, const char *errMsg, va_list args)
{
    if(fileId)
    {
        FILE_STATE fileState;
        manager->GetFileState(fileId, &fileState);
        char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
        memcpy(pathBuffer, fileState.path, fileState.pathLen);
        pathBuffer[fileState.pathLen] = '\0';
        printf("%s:", pathBuffer);
    }
    if(loc)
    {
        printf("%ld:%ld", loc->line, loc->offset);
    }
    printf(" Parser warning \n");

    if(errMsg)
    {
        vprintf(errMsg, args);
    }
    printf("\n");
}

Ast::Node *Parser::AllocateAstNodes(uint16_t count)
{
    size_t memorySize = count * sizeof(Ast::Node);
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
    PP.StartConstantExpr();
    Ast::Node* node = ConditionalExpression();
    PP.StopConstantExpr();
    pState.parsingConstantExpr = 0;
    return node;
}

Ast::Node* Parser::ParseExpression()
{
    return AssignmentExpression();
}

Ast::Node* Parser::PrimaryExpression()
{
    static constexpr const char* expectedStrs[] = {
        tokenStr(TokenType::identifier), tokenStr(TokenType::numeric_constant),
        tokenStr(TokenType::string_literal), tokenStr(TokenType::l_string_literal),
        tokenStr(TokenType::character_literal), tokenStr(TokenType::l_parentheses)};

    Token token = GetCurrToken();
    ConsumeToken();
    Ast::Node* node = AllocateAstNodes(1);
    node->token = token;
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
        node->type = Ast::expression;
        node->lChild = ParseExpression();
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

Ast::Node* Parser::PostfixExpression()
{
    Ast::Node* postfixExpr = PrimaryExpression();
    Token token = GetCurrToken();
    if(token.type == TokenType::l_parentheses)
    {
        ConsumeToken();
        IssueWarning(nullptr, "Currently parsing type-name/initializer-list is not supported \n");
    }

    while (IsTokenOneOf(&token, TokenType::l_bracket, TokenType::l_parentheses, 
            TokenType::dot, TokenType::arrow, TokenType::plus_plus, TokenType::minus_minus))
    {
        Ast::Node* node = AllocateAstNodes();
        node->token = token;
        
        if(token.type == TokenType::l_bracket)
        {
            node->type = Ast::NodeType::array_access;
            node->lChild = postfixExpr;
            node->rChild = ParseExpression();
        }
        else if(token.type == TokenType::l_parentheses)
        {
            node->type = Ast::NodeType::function_call;
            node->lChild = postfixExpr;
            IssueWarning(&node->token, "Function calls are currenlty not supported \n");
            exit(-1);
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

    while (IsTokenOneFromArray(&token, tokTypes))
    {
        if(IsTokenOneFromArray(&token, tokTypes))
        {
            ConsumeToken();
            Ast::Node* node =  AllocateAstNodes();
            node->type = ResolveNodeType(token.type, tokTypes, opTypes);
            node->lChild = CastExpression();
            node->rChild = nullptr;
            node->token = token;
            return node;
        }
    }
    
    return PostfixExpression();
}

Ast::Node* Parser::CastExpression()
{
    Ast::Node* unary;
    if(unaryHandle)
    {
        unary = unaryHandle;
        unaryHandle = nullptr;
    }
    else
    {
        unary = UnaryExpression();
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
    return LogicalOrExpression();
}

Ast::Node* Parser::AssignmentExpression()
{
    Ast::Node* unary = UnaryExpression();
    Token token = GetCurrToken();
    if(!IsAssignment(&token))
    {
        unaryHandle = unary;
        return ConditionalExpression();
    }
    exit(-1);
    return nullptr;
}


