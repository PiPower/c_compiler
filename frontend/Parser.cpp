#include "Parser.hpp"
#include <cassert>
#include <sys/mman.h> 
#include <string.h>
#include <stdarg.h>
#define CPU_PAGE 4096
#define PAGE_COUNT 50

Parser::Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
manager(manager), PP(mainFile, manager, opts), opts(opts), unaryHandle(nullptr), pState({})
{
    assert(opts != nullptr);
    AddNodePage();
}

void Parser::Parse()
{
start_parsing:
    Token token = GetCurrToken();
    
    if(PP.stages.If > 0)
    {
        Ast::Node* expr = ParseConstantExpr();
        PP.ExecuteConstantExpr(expr);
        goto start_parsing;
    }

    while (token.type != TokenType::eof)
    {
        PP.Peek(&token);
    }

}

Token Parser::GetCurrToken()
{
    if(tokenQueue.empty())
    {
        Token token;
        PP.Peek(&token);
        tokenQueue.push_back(token);
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
        IssueWarning(&token.location.id, &token.location, 
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

void Parser::IssueWarning(const FILE_ID *fileId, const SourceLocation *loc, const char *errMsg, ...)
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
        va_list args;
        va_start(args, errMsg);
        vprintf(errMsg, args);
        va_end(args);
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
    Ast::Node* node = ConditionalExpression();
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
        tokenStr(TokenType::l_parentheses)};

    Token token = GetCurrToken();
    ConsumeToken();
    Ast::Node* node = AllocateAstNodes(1);
    node->token = token;
    switch (token.type)
    {
    case TokenType::identifier:       node->type = Ast::identifier; break;
    case TokenType::numeric_constant: node->type = Ast::constant; break;
    case TokenType::string_literal:
    case TokenType::l_string_literal: 
        node->type = Ast::string_literal;
        break;
    case TokenType::l_parentheses:
        node->type = Ast::expression;
        node->lChild = ParseExpression();
        ConsumeExpectedToken(TokenType::r_parentheses);
        break;
    default:
        IssueWarning(&token, " Given token is [%s] but only [%s], [%s], [%s], [%s] or [%s] can be used in primary expression",
            tokenStr(token.type), expectedStrs[0], expectedStrs[1], expectedStrs[2], expectedStrs[3], expectedStrs[4]);
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
    Token token = GetCurrToken();
    if(token.type == TokenType::l_parentheses)
    {
        ConsumeToken();
        IssueWarning(nullptr, "Currently parsing type-name/initializer-list is not supported \n");
    }

    Ast::Node* postfixExpr = PrimaryExpression();
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
    return CastExpression();
}

Ast::Node* Parser::AdditiveExpression()
{
    return MultiplicativeExpression();
}

Ast::Node* Parser::ShiftExpression()
{
    return AdditiveExpression();
}

Ast::Node* Parser::RelationalExpression()
{
    return ShiftExpression();
}

Ast::Node* Parser::EqualityExpression()
{
    return RelationalExpression();
}

Ast::Node* Parser::AndExpression()
{
    return EqualityExpression();
}

Ast::Node* Parser::ExclusiveOrExpression()
{
    return AndExpression();
}

Ast::Node* Parser::InclusiveOrExpression()
{
    return ExclusiveOrExpression();
}

Ast::Node* Parser::LogicalAndExpression()
{
    return InclusiveOrExpression();
}

Ast::Node* Parser::LogicalOrExpression()
{
    return LogicalAndExpression();
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


