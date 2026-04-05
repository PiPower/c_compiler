#include "Parser.hpp"
#include <cassert>
#include <sys/mman.h> 
#include <string.h>
#include <stdarg.h>
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
analyzer(analyzer), manager(manager), PP(mainFileId, manager, opts), opts(opts), unaryHandle(nullptr), pState({})
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

    Ast::Node* declSpec = ParseDeclSpec();
    Ast::Node* initDeclarationList = ParseInitDeclList();
    token = GetCurrToken();

    ConsumeExpectedToken(TokenType::semicolon);
    Ast::Node* declaration = AllocateAstNodes();
    declaration->type = Ast::declaration;
    declaration->lChild = declSpec;
    declaration->rChild = initDeclarationList;
    return declaration;
}

Token Parser::GetCurrToken()
{
    // this is temporary solution to skip kw__attribute__
    // to be resolved later
    Token token = GetCurrTokenInternal();
    if(token.type == TokenType::kw__attribute__)
    {
        ConsumeToken();
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
    return GetCurrTokenInternal();
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

    currentLocation = tokenQueue.front().location;
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
    va_list args;
    va_start(args, errMsg);
    IssueWarning(fileId, loc, errMsg, args);
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

Ast::Node *Parser::ParseInitializer()
{
    return nullptr;
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

        Ast::Node* declGlue = AllocateAstNodes();
        declGlue->type = Ast::glue_list;
        declGlue->lChild = initDeclarator;
        bottomChild->rChild = declGlue;
        bottomChild = declGlue;

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
            Ast::Node* array = ParseFunctionCallArgs();
            array->token = token;
            ConsumeExpectedToken(TokenType::r_parentheses);

            Ast::Node* glue = AllocateAstNodes();
            glue->type = Ast::glue_list;
            glue->lChild = array;
            bottomChild->rChild = glue;
            bottomChild = glue;
        }

        if(token.type == TokenType::l_bracket)
        {
            ConsumeExpectedToken(TokenType::l_bracket);
            Ast::Node* array = ParseArrayArgs();
            array->token = token;
            ConsumeExpectedToken(TokenType::r_bracket);

            Ast::Node* glue = AllocateAstNodes();
            glue->type = Ast::glue_list;
            glue->lChild = array;
            bottomChild->rChild = glue;
            bottomChild = glue;
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

    typeName->rChild = AbstractDeclarator();
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
        Ast::Node* gluNodeDecl = AllocateAstNodes();
        gluNodeDecl->type = Ast::NodeType::glue_list;
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
        glueNode->type = Ast::NodeType::glue_list;
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
        Ast::Node* declSpec = ParseDeclSpec();
        Ast::Node* ptrExpr = ParsePointer();
        Ast::Node* declarator = ParseDirectDeclarator();
        if(!declarator)
        {
            declarator = ParseDirectAbstractDeclarator();
        }
        if(declarator)
        {
            declarator->lChild = ptrExpr;
        }
        if(ptrExpr && !declarator)
        {
            // this is direct abstract declarator 
            declarator = AllocateAstNodes();
            declarator->type = Ast::NodeType::abstact_declarator;
            declarator->rChild = ptrExpr;
        }
        if(!declSpec )
        {
            IssueWarning(nullptr, &currentLocation, "function call declaration specifier cannot be empty" );
            exit(-1);
        }
        Ast::Node *parameterDecl = AllocateAstNodes();
        parameterDecl->type = Ast::parameter_decl;
        parameterDecl->lChild = declSpec;
        parameterDecl->rChild = declarator;

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
            printf("TokenType::l_parentheses, are not supported for abstract declarator\n");
            exit(-1);
            Ast::Node* directAbstractDeclarator = nullptr;
            Ast::Node* parameterTypeList = ParameterTypeList();
            if(!parameterTypeList)
            {
                directAbstractDeclarator =  AbstractDeclarator();
            }
        }
        else
        {
            Ast::Node* array = AllocateAstNodes();
            array->token = token;
            array->type = Ast::array_decl;
            array->lChild = AssignmentExpression();

            Token star = GetCurrToken();
            if(!array->lChild && star.type == TokenType::star)
            {
                array->type = Ast::var_len_array;
                ConsumeExpectedToken(TokenType::star);
            }
            ConsumeExpectedToken(TokenType::r_bracket);

            bottomChild->rChild = array;
            bottomChild = array;
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

        Ast::Node *glueList = AllocateAstNodes();
        glueList->type = Ast::glue_list;
        glueList->lChild = parameterDecl;

        bottomChild->rChild = glueList;
        bottomChild = glueList;

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
            node->token = token;
            ConsumeExpectedToken(TokenType::r_parentheses);

        }
        else
        {
            node->lChild = UnaryExpression();
        }
        return node;
    }


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


