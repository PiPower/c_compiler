#include "SemanticAnalysis.hpp"
#include <string.h>
#include <limits>

typedef const Ast::Node Node;

static const char* kTypeNames[] = {
    // void
    "void",
    // char
    "char", "signed char", "unsigned char",
    // short
    "short", "signed short",
    "short int", "signed short int",
    "unsigned short", "unsigned short int",
    // int
    "int", "signed", "signed int",
    "unsigned", "unsigned int",
    // long
    "long", "signed long",
    "long int", "signed long int",
    "unsigned long", "unsigned long int",
    "long unsigned int",
    // long long
    "long long", "signed long long",
    "long long int", "signed long long int",
    "unsigned long long", "unsigned long long int",
    // floating
    "float", "double", "long double",
    // other
    "_Bool",
    "float _Complex", "double _Complex", "long double _Complex",
    // special
    "__builtin_va_list"
};
SemanticAnalyzer::SemanticAnalyzer(FileManager* manager, SymbolTable* symTab)
:
symTab(symTab), manager(manager)
{
    // set offset into max so that new page gets allocated
    handyString.reserve(100);
 
    // each simple built in typename is stored in read section during program lifetime
    // so each std::string_view will be valid
    symTab->AddSymbol<SymbolType>(kTypeNames[0], BuiltIn::void_t, 0, nullptr, nullptr, nullptr);

    // integer types
    symTab->AddSymbol<SymbolType>(kTypeNames[1], BuiltIn::s_char_8, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[2], BuiltIn::s_char_8, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[3], BuiltIn::u_char_8, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[4], BuiltIn::s_int_16, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[5], BuiltIn::s_int_16, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[6], BuiltIn::s_int_16, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[7], BuiltIn::s_int_16, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[8], BuiltIn::u_int_16, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[9], BuiltIn::u_int_16, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[10], BuiltIn::s_int_32, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[11], BuiltIn::s_int_32, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[12], BuiltIn::s_int_32, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[13], BuiltIn::u_int_32, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[14], BuiltIn::u_int_32, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[15], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[16], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[17], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[18], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[19], BuiltIn::u_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[20], BuiltIn::u_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[21], BuiltIn::u_int_64, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[22], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[23], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[24], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[25], BuiltIn::s_int_64, 0, nullptr, nullptr, nullptr);

    symTab->AddSymbol<SymbolType>(kTypeNames[26], BuiltIn::u_int_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[27], BuiltIn::u_int_64, 0, nullptr, nullptr, nullptr);

    // floats
    symTab->AddSymbol<SymbolType>(kTypeNames[28], BuiltIn::float_32, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[29], BuiltIn::double_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[30], BuiltIn::long_double, 0, nullptr, nullptr, nullptr);

    // rest
    symTab->AddSymbol<SymbolType>(kTypeNames[31], BuiltIn::bool_t, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[32], BuiltIn::complex_float_64, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[33], BuiltIn::complex_double_128, 0, nullptr, nullptr, nullptr);
    symTab->AddSymbol<SymbolType>(kTypeNames[34], BuiltIn::complex_long_double, 0, nullptr, nullptr, nullptr);

    // special built-in
    symTab->AddSymbol<SymbolType>(kTypeNames[35], BuiltIn::special, 0, nullptr, nullptr, nullptr);

}

void SemanticAnalyzer::Analyze(const Ast::Node *root)
{
    if(root->type == Ast::declaration)
    {
        AnalyzeDeclaration(root->lChild, root->rChild);
        return;
    }
}

void SemanticAnalyzer::AnalyzeDeclaration(const Ast::Node *declSpecs, const Ast::Node *initDeclList)
{
    DeclSpecs declaration = AnalyzeDeclSpec(declSpecs->rChild);
    if(declaration.declType.spec.typedef_)
    {
        AnalyzeTypedef(&declaration, initDeclList);
    }
}

StructDeclaration SemanticAnalyzer::AnalyzeStructDeclaration(const Ast::Node *declSpecs, const Ast::Node* structDeclList)
{
    StructDeclaration structDecl;
    structDecl.declSpec = AnalyzeDeclSpec(declSpecs);
    Node* currentNode = structDeclList ;
    while (currentNode)
    {
        Node* structDeclarator = currentNode->lChild;
        // InitDeclarator without initializer is just declarator
        InitDeclarator initDecl = AnalyzeDeclarator(structDeclarator->lChild, nullptr);
        int64_t bitCount = -1;
        if(structDeclarator->rChild)
        {
            Typed::Number constExpr;
            memcpy(&constExpr, &structDeclarator->rChild->lChild, sizeof(Typed::Number));
            if(constExpr.type != Typed::d_int64_t){printf("constant expr has not allowed type \n"); exit(-1);}
            bitCount = constExpr.int64;
        }

        StructDeclarator decl;
        decl.decl = initDecl.decl;
        decl.bitCount = bitCount;
        structDecl.declarators.push_back(decl);
        currentNode = currentNode->rChild;
    }
    
    return structDecl;
}

void SemanticAnalyzer::AnalyzeTypedef(DeclSpecs* declSpec, const Ast::Node *initDeclList)
{
    Node* root = initDeclList;
    uint16_t symKind = symTab->QuerySymKinds(declSpec->typenameView);
    if( (symKind & (Sym::TYPEDEF | Sym::TYPE)) == 0)
    {
        printf("Specified identifier does not name a type\n");
        exit(-1);
    }

    while (Node* currChild = root->rChild)
    {
        Ast::Node* initDecl = currChild->lChild;
        InitDeclarator iDecl = AnalyzeDeclarator(initDecl->rChild, initDecl->lChild);

        if(iDecl.initializer)
        {
            printf("typedef is not allowed to have initializer\n");
            exit(-1);
        }
        
        symTab->AddSymbol<SymbolTypedef>(iDecl.decl.name, declSpec->typenameView);

        root = currChild;
    }
    
}

void SemanticAnalyzer::AnalyzeUnion(const Ast::Node *unionTree, DeclSpecs *spec)
{
    printf("Unsupported union\n");
    exit(-1);
}

void SemanticAnalyzer::AnalyzeStruct(const Ast::Node *structTree, DeclSpecs *spec)
{
    if(structTree->lChild)
    {
        spec->typenameView = GetViewForToken(structTree->lChild->token);
    }
    else
    {
        
    }

    if(!structTree->rChild)
    {
        // test whether its declaration of type or object of said type
        return;
    }

    // register name, its gonna be used to test redefinition    
    symTab->AddSymbol<SymbolType>(spec->typenameView, BuiltIn::struct_t, 0, nullptr, nullptr, nullptr);
    // struct has its own scope
    symTab->CreateNewScope(Scope::STRUCT);
    ScopedSymbolTable* structSymtab = symTab->currentTable;
    Node *argList = structTree;
    std::vector<StructDeclaration> structDecls;
    size_t argCount = 0;
    while ((argList = argList->rChild))
    {
        Node * structDeclPtr = argList->lChild;
        StructDeclaration structDecl = AnalyzeStructDeclaration(structDeclPtr->lChild, structDeclPtr->rChild);
        structDecls.push_back(structDecl);
        argCount += structDecl.declarators.size();
    }
    ScopedSymbolTable* scopedTable = symTab->currentTable;
    symTab->PopScope();
    std::string_view* argNames = symTab->AllocateTypeArrayOnHeap<std::string_view>(argCount);
    Member* members = symTab->AllocateTypeArrayOnHeap<Member>(argCount);
    size_t idx = 0;
    for(size_t i = 0; i < structDecls.size(); i++)
    {
        for(size_t j = 0; j < structDecls[i].declarators.size(); j++)
        {
            members[idx].declType = structDecls[i].declSpec.declType;
            members[idx].typeName = structDecls[i].declSpec.typenameView;
            members[idx].bitCount = structDecls[i].declarators[j].bitCount;
            argNames[idx] = structDecls[i].declarators[j].decl.name;
            idx++;
        }
    }
    SymbolType* sym = symTab->QueryTypeSymbol(spec->typenameView);
    sym->structTable = scopedTable;
    sym->argCount = argCount;
    sym->memberNames = argNames;
    sym->memberList = members;

    return; 
}

InitDeclarator SemanticAnalyzer::AnalyzeDeclarator(const Ast::Node *declarator, const Ast::Node *initializer)
{
    InitDeclarator initDecl = {};
    initDecl.initializer = initializer;

    Node* ptrDecl = declarator->rChild;
    if(declarator->lChild->type == Ast::direct_declarator)
    {
        Node* directDeclarator = declarator->lChild;
        if(directDeclarator->lChild)
        {
            printf("Nested direct-declarator is not supported \n");
            exit(-1);
        }
        initDecl.decl.name = GetViewForToken(directDeclarator->token);

        if(directDeclarator->rChild)
        {
            printf("arrays/callings are not supported\n");
            exit(-1);
        }

    }
    else
    {
        printf("Abstract declarator is not supported \n");
        exit(-1);
    }
    return initDecl;
}

void SemanticAnalyzer::AnalyzeEnum(const Ast::Node *enumTree, DeclSpecs *spec)
{
    printf("Unsupported enum\n");
    exit(-1);
}

void SemanticAnalyzer::AnalyzeSimpleType(const Ast::Node *typeSequence, DeclSpecs *spec)
{
    handyString.clear();
    Node* currChild = typeSequence;
    do
    {
        handyString += GetViewForToken(currChild->token);
        currChild = currChild->lChild;
        if(currChild)
        {
           handyString += ' '; 
        }
    }while (currChild);

    int idx = -1;
    for(int i = 0; i < (int) (sizeof(kTypeNames)/sizeof(const char*)); i++)
    {
        const char* tName = kTypeNames[i];
        if(strcmp(tName, handyString.data()) == 0)
        {
            idx = i;
            break;
        }
    }

    if(idx == -1)
    {
        printf("Unrecognized simple type \n");
        exit(-1);
    }
    spec->typenameView = std::string_view(kTypeNames[idx]);
}

std::string_view SemanticAnalyzer::GetViewForToken(const Token &token)
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

bool SemanticAnalyzer::IsAliasOfType(const std::string_view& identifier)
{
    return (symTab->QuerySymKinds(identifier) & (Sym::TYPEDEF | Sym::TYPE) ) > 0;
}

uint64_t SemanticAnalyzer::GetAnnonymousId()
{
    static uint64_t id = 0;
    return id++;
}

DeclSpecs SemanticAnalyzer::AnalyzeDeclSpec(const Ast::Node *declSpecs)
{
    DeclSpecs spec = {};

    Node* currNode = declSpecs;
    while (currNode)
    {
        if(currNode->type == Ast::type_qualifier)
        {
            // 6.7.3 Type qualifiers
            switch (currNode->token.type)
            {
            case TokenType::kw_const:
                spec.declType.qual.const_ = 1;
                break;
            case TokenType::kw_restrict:
                spec.declType.qual.restrict_ = 1;
                break;
            case TokenType::kw_volatile:
                spec.declType.qual.volatile_ = 1;             
                break;
            default:
                printf("Incorrect type qualifier \n");
                exit(-1);
                break;
            }
        }
        else if(currNode->type == Ast::storage_specifier)
        {
            // 6.7.1 Storage-class specifiers
            bool redaclarationError = false;
            bool moreThanOne = false;
            if(spec.declType.storageFlags > 0){ moreThanOne = true;}
            switch (currNode->token.type)
            {
            case TokenType::kw_register:
                if(spec.declType.spec.register_ > 0){ redaclarationError = true;}
                spec.declType.spec.register_ = 1;
                break;
            case TokenType::kw_typedef:
                if(spec.declType.spec.typedef_ > 0){ redaclarationError = true;}
                spec.declType.spec.typedef_ = 1;
                break;
            case TokenType::kw_static:
                if(spec.declType.spec.static_ > 0){ redaclarationError = true;}
                spec.declType.spec.static_ = 1;             
                break;
            case TokenType::kw_extern:
                if(spec.declType.spec.extern_ > 0){ redaclarationError = true;}
                spec.declType.spec.extern_ = 1; 
                break;
            case TokenType::kw_auto:
                if(spec.declType.spec.auto_ > 0){ redaclarationError = true;}
                spec.declType.spec.auto_ = 1;
                break;
            default:
                printf("Incorrect storage specifier \n");
                exit(-1);
                break;
            }
            if(moreThanOne)
            {
                printf("More than one storage_specifier is not allowed \n");
                exit(-1);
            }
            if(redaclarationError)
            {
                printf("Storage specifier is not allowed to be repeated \n");
                exit(-1);
            }

        }
        else if(currNode->type == Ast::type_specifier)
        {
            switch (currNode->token.type)
            {
            case TokenType::kw_union:
                AnalyzeUnion(currNode->lChild, &spec);
                break;
            case TokenType::kw_struct:
                AnalyzeStruct(currNode->lChild, &spec);
                break;
            case TokenType::kw_enum:
                AnalyzeEnum(currNode->lChild, &spec);
                break;
            default:
                AnalyzeSimpleType(currNode, &spec);
                break;
            }
            // in case of type specifiers left subtree has meaning 
            // related strictly related to type specifier so it should not be crawled
            currNode = currNode->rChild;
            continue;
        }
        else if(currNode->type == Ast::function_specifier)
        {
            // 6.7.4 Function specifiers
            spec.declType.inlineSpec = 1;
        }
        else
        {
            printf("Incorrect declaration specifier \n");
            exit(-1);
        }


        if(currNode->lChild)
        {
            currNode = currNode->lChild;
        }
        else
        {
            currNode = currNode->rChild;
        }
    }

    return spec;
}
