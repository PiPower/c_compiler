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

Parser::Parser(FILE_ID mainFileId, FileManager* manager, const CompilationOpts* opts)
:
manager(manager), PP(mainFileId, manager, opts), opts(opts), unaryHandle(nullptr), pState({})
{
    assert(opts != nullptr);
    AddNodePage(); 
}

void Parser::Parse()
{

    Ast::Node* declSpec = ParseDeclSpec();


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

        if(token.type == TokenType::eof)
        {
            return;
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
    if(pState.parsingConstantExpr == 1 && tokenQueue.front().type == TokenType::line_splice )
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
Ast::Node *Parser::ParseDeclarator()
{
    Ast::Node* ptrExpr = ParsePointer();
    Ast::Node* declExpr = ParseDirectDeclarator();
    if(declExpr)
    {
        Ast::Node* declarator = AllocateAstNodes();
        declarator->type = Ast::NodeType::declarator;
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


    Ast::Node* ptr = AllocateAstNodes();
    ptr->lChild = TypeQualifierList();
    ptr->type = Ast::pointer;
    ptr->token = token;
    Ast::Node* bottom = ptr;
    
    ConsumeToken();
    token = GetCurrToken();
    while (token.type == TokenType::star)
    {   
        Ast::Node* subPtr = AllocateAstNodes();
        subPtr->type = Ast::pointer;
        subPtr->lChild = TypeQualifierList();
        subPtr->token = token;
        bottom->rChild = subPtr;
        bottom = subPtr;
        
        ConsumeToken();
        token = GetCurrToken();
    }
    return ptr;
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

    AssmExpr = AssignmentExpression();
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
    Ast::Node* directDeclarator = AllocateAstNodes();
    directDeclarator->type = Ast::direct_declarator;
    directDeclarator->token = token;

    if(token.type == TokenType::l_parentheses)
    {
        ConsumeExpectedToken(TokenType::l_parentheses);
        directDeclarator->lChild = ParseDeclarator();
        ConsumeExpectedToken(TokenType::r_parentheses);
    }
    else if(token.type == TokenType::identifier)
    {
        ConsumeToken();
    }

    token = GetCurrToken();
    while (IsTokenOneOf(&token, TokenType::l_bracket, TokenType::l_parentheses))
    {
        if(token.type == TokenType::l_parentheses)
        {
            printf("declarator of calls/arrays not implemented\n");
            exit(-1);
        }

        if(token.type == TokenType::l_bracket)
        {
            ConsumeExpectedToken(TokenType::l_bracket);
            Ast::Node* array = ParseArrayArgs();
            array->token = token;
            ConsumeExpectedToken(TokenType::r_bracket);
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

    while (keepParsing)
    {
        keepParsing = false;
        // right child of each top level decl ast is RESERVED for chaining
        if(Ast::Node* subDecl = DeclSpecSubtype())
        {
            keepParsing = true;
            bottomChild->rChild = subDecl;
            bottomChild = bottomChild->rChild;
        }
    }
    
    return declSpec;
}

Ast::Node *Parser::DeclSpecSubtype()
{
    if (Ast::Node* nodeOut = StorageSpec())
    {
        return nodeOut;
    }
    if (Ast::Node* nodeOut = TypeSpecifier())
    {
        return nodeOut;
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

Ast::Node *Parser::TypeSpecifier()
{
    constexpr auto storageSpecifiers = std::to_array<TokenType::Type>({TokenType::kw_void, TokenType::kw_char, 
        TokenType::kw_short, TokenType::kw_int, TokenType::kw_long, TokenType::kw_float,
        TokenType::kw_double, TokenType::kw_signed, TokenType::kw_unsigned, TokenType::kw_bool,
        TokenType::kw_complex, TokenType::kw_imaginary});

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
        // currently not supported we need symbol table for this
        return nullptr;
        //ConsumeToken();
        //Ast::Node* node = AllocateAstNodes();
        //node->type = Ast::typedef_name;
        //node->token = token;
        //return node;
    }
    
    return nullptr;
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
    glueNode->type = Ast::NodeType::glue;
    topLevelNode->lChild = glueNode;

    Token token = GetCurrToken();
    if(token.type == TokenType::identifier)
    {
        ConsumeToken();
        Ast::Node* identifier = AllocateAstNodes();
        identifier->type=Ast::NodeType::identifier;
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
        Ast::Node* gluNodeDecl = AllocateAstNodes();
        gluNodeDecl->type = Ast::NodeType::glue;
        gluNodeDecl->lChild = structDecl;
        bottomChild->rChild = gluNodeDecl;
        bottomChild = gluNodeDecl;

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
    return nullptr;
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
        Ast::Node* glueNode = AllocateAstNodes();
        glueNode->type = Ast::NodeType::glue;
        glueNode->lChild = declarator;
        bottomChild->rChild = glueNode;
        bottomChild = glueNode;
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
    }

    Ast::Node* structDecl = AllocateAstNodes();
    structDecl->type = Ast::struct_declarator;
    structDecl->lChild = declarator;
    structDecl->rChild = constantExpr;
    return structDecl;
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


