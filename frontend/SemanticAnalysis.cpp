#include "SemanticAnalysis.hpp"
#include <string.h>
#include <limits>

SemanticAnalyzer::SemanticAnalyzer(FileManager* manager, SymbolTable* symTab)
:
manager(manager), symTab(symTab)
{
    // set offset into max so that new page gets allocated
    compoundTypeStr.reserve(100);

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
    DeclSpecs declaration = AnalyzeDeclSpec(declSpecs);
    if(declaration.declType.spec.typedef_)
    {
        AnalyzeTypedef(&declaration, initDeclList);
    }
}

void SemanticAnalyzer::AnalyzeTypedef(DeclSpecs* declSpec, const Ast::Node *initDeclList)
{
    
    int x = 2;
    
}

void SemanticAnalyzer::AnalyzeUnion(const Ast::Node *unionTree, DeclSpecs *spec)
{
    printf("Unsupported union\n");
    exit(-1);
}

void SemanticAnalyzer::AnalyzeStruct(const Ast::Node *structTree, DeclSpecs *spec)
{
    printf("Unsupported struct\n");
    exit(-1);
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

    spec->typenameHandle = compoundTypeStr.c_str();
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


bool SemanticAnalyzer::AliasOfType(const std::string_view identifier)
{
    return false;
}

DeclSpecs SemanticAnalyzer::AnalyzeDeclSpec(const Ast::Node *declSpecs)
{
    DeclSpecs spec = {};
    compoundTypeStr.clear();

    const Ast::Node* currNode = declSpecs->rChild;
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
