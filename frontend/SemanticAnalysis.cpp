#include "SemanticAnalysis.hpp"
#include <string.h>
#include <limits>

SemanticAnalyzer::SemanticAnalyzer(FileManager* manager, SymbolTable* symTab)
:
symTab(symTab), manager(manager)
{
    // set offset into max so that new page gets allocated
    compoundTypeStr.reserve(100);
 
    // each simple built in typename is stored in read section during program lifetime
    // so each std::string_view will be valid
    symTab->AddSymbol<SymbolType>("void", BuiltIn::void_t, TypeBits{}, 0, nullptr);
    // integer types
    symTab->AddSymbol<SymbolType>("char", BuiltIn::s_char_8, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed char", BuiltIn::s_char_8, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("unsigned char", BuiltIn::u_char_8, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("short", BuiltIn::s_int_16, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed short", BuiltIn::s_int_16, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("short int", BuiltIn::s_int_16, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed short int", BuiltIn::s_int_16, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("unsigned short", BuiltIn::u_int_16, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("unsigned short int", BuiltIn::u_int_16, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("int", BuiltIn::s_int_32, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed", BuiltIn::s_int_32, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed int", BuiltIn::s_int_32, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("unsigned", BuiltIn::u_int_32, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("unsigned int", BuiltIn::u_int_32, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("long", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed long", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("long int", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed long int", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("unsigned long", BuiltIn::u_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("unsigned long int", BuiltIn::u_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("long unsigned int", BuiltIn::u_int_64, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("long long", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed long long", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("long long int", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("signed long long int", BuiltIn::s_int_64, TypeBits{}, 0, nullptr);

    symTab->AddSymbol<SymbolType>("unsigned long long", BuiltIn::u_int_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("unsigned long long int", BuiltIn::u_int_64, TypeBits{}, 0, nullptr);
    // floats
    symTab->AddSymbol<SymbolType>("float", BuiltIn::float_32, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("double", BuiltIn::double_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("long double", BuiltIn::long_double, TypeBits{}, 0, nullptr);
    // rest
    symTab->AddSymbol<SymbolType>("_Bool", BuiltIn::bool_t, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("float _Complex", BuiltIn::complex_float_64, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("double _Complex", BuiltIn::complex_double_128, TypeBits{}, 0, nullptr);
    symTab->AddSymbol<SymbolType>("long double _Complex", BuiltIn::complex_long_double, TypeBits{}, 0, nullptr);
    // special built-in
    symTab->AddSymbol<SymbolType>("__builtin_va_list", BuiltIn::special, TypeBits{}, 0, nullptr);

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
    const Ast::Node* currentNode = structDeclList ;
    while (currentNode)
    {
        const Ast::Node* structDeclarator = currentNode->lChild;
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
    const Ast::Node* root = initDeclList;
    Sym::Kind symKind = symTab->QuerySymKind(declSpec->typenameView);
    if(symKind != Sym::TYPEDEF && symKind != Sym::TYPE)
    {
        printf("Specified identifier does not name a type\n");
        exit(-1);
    }

    while (const Ast::Node* currChild = root->rChild)
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
    std::string_view structName;
    if(structTree->lChild)
    {
        structName = GetViewForToken(structTree->lChild->token);;
    }
    const Ast::Node *argList = structTree;
    while ((argList = argList->rChild))
    {
        const Ast::Node * structDeclPtr =argList->lChild;
        StructDeclaration structDecl = AnalyzeStructDeclaration(structDeclPtr->lChild, structDeclPtr->rChild);
        int x = 2;
    }
    

    printf("Unsupported struct\n");
    exit(-1);
}

InitDeclarator SemanticAnalyzer::AnalyzeDeclarator(const Ast::Node *declarator, const Ast::Node *initializer)
{
    InitDeclarator initDecl = {};
    initDecl.initializer = initializer;

    const Ast::Node* ptrDecl = declarator->rChild;
    if(declarator->lChild->type == Ast::direct_declarator)
    {
        const Ast::Node* directDeclarator = declarator->lChild;
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

    const Ast::Node* currChild = typeSequence;
    do
    {
        compoundTypeStr += GetViewForToken(currChild->token);
        currChild = currChild->lChild;
        if(currChild)
        {
           compoundTypeStr += ' '; 
        }
    }while (currChild);

    spec->typenameView = compoundTypeStr;
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
    Sym::Kind symKind = symTab->QuerySymKind(identifier);
    return symKind == Sym::TYPEDEF || symKind == Sym::TYPE;
}

DeclSpecs SemanticAnalyzer::AnalyzeDeclSpec(const Ast::Node *declSpecs)
{
    DeclSpecs spec = {};
    compoundTypeStr.clear();

    const Ast::Node* currNode = declSpecs;
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
