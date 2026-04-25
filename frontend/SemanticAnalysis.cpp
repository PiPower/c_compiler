#include "SemanticAnalysis.hpp"
#include <string.h>
#include <limits>
#include <fcntl.h>
#include <unistd.h>

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
symTab(symTab), manager(manager), codeGen(symTab, manager)
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
    symTab->AddSymbol<SymbolType>(kTypeNames[31], BuiltIn::u_char_8, 0, nullptr, nullptr, nullptr);
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
    DeclSpecs declSpec = AnalyzeDeclSpec(declSpecs->rChild);
    if(declSpec.declType.spec.typedef_)
    {
        AnalyzeTypedef(&declSpec, initDeclList);
        return;
    }

    if(initDeclList)
    {
        AnalyzeInitDeclList(&declSpec, initDeclList);
    }

    // if empty decl spec then do nothing 
    
}

StructDeclaration SemanticAnalyzer::AnalyzeStructDeclaration(const Ast::Node *declSpecs, const Ast::Node* structDeclList)
{
    StructDeclaration structDecl;
    structDecl.declSpec = AnalyzeDeclSpec(declSpecs);
    Node* currentNode = structDeclList ;

    bool NoDeclarators = true;
    while (currentNode)
    {
        Node* structDeclarator = currentNode->lChild;
        // InitDeclarator without initializer is just declarator
        if(!structDeclarator->lChild)
        {
            currentNode = currentNode->rChild;
            continue;
        }
        NoDeclarators = false;
        InitDeclarator initDecl;
        initDecl.decl = AnalyzeDeclarator(structDeclarator->lChild);
        int64_t bitCount = -1;
        if(structDeclarator->rChild)
        {
            Typed::Number constExpr;
            memcpy(&constExpr, &structDeclarator->rChild->lChild, sizeof(Typed::Number));
            if(constExpr.type != Typed::d_int64_t){printf("constant expr has not allowed type \n"); exit(-1);}
            bitCount = constExpr.int64;
        }

        StructDeclarator decl = {};
        decl.decl = initDecl.decl;
        decl.bitCount = bitCount;
        structDecl.declarators.push_back(decl);
        currentNode = currentNode->rChild;
    }
    
    if(NoDeclarators && structDecl.declSpec.typenameView.length() > 6)
    {
        std::string_view substr = structDecl.declSpec.typenameView.substr(0, 6) ;
        if(substr == "%union")
        {
            static uint64_t anonMemberUnion = 0;
            StructDeclarator decl = {};

            std::string name = "%anon.member_union." + std::to_string(anonMemberUnion++);
            decl.decl.name = symTab->AddSymbolName(name.c_str());
            decl.bitCount = -1;
            structDecl.declarators.push_back(decl);
        }
    }

    if(NoDeclarators && structDecl.declSpec.typenameView.length() > 7)
    {
        std::string_view substr = structDecl.declSpec.typenameView.substr(0, 7) ;
        if(substr == "%struct")
        {
            static uint64_t anonMemberStruct = 0;
            StructDeclarator decl = {};

            std::string name = "%anon.member_struct." + std::to_string(anonMemberStruct++);
            decl.decl.name = symTab->AddSymbolName(name.c_str());
            decl.bitCount = -1;
            structDecl.declarators.push_back(decl);
        }
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
        //większa walidacja 
        Ast::Node* initDecl = currChild->lChild;
        InitDeclarator iDecl;
        iDecl.decl = AnalyzeDeclarator(initDecl->rChild);
        iDecl.initializer =  initDecl->lChild;

        if(iDecl.initializer)
        {
            printf("typedef is not allowed to have initializer\n");
            exit(-1);
        }
        
        symTab->AddSymbol<SymbolTypedef>(iDecl.decl.name, declSpec->typenameView, declSpec->declType.qual);

        root = currChild;
    }
    
}

void SemanticAnalyzer::AnalyzeStructUnion(const Ast::Node *structTree, DeclSpecs *spec, bool isStruct)
{
    if(structTree->lChild)
    {
        spec->typenameView = GetViewForToken(structTree->lChild->token);
    }
    else
    {
        std::string anonName;
        if(isStruct)
        {
            anonName = "%struct.anon." + std::to_string(GetAnnonymousStructId());
        }
        else
        {
            anonName = "%union.anon." + std::to_string(GetAnnonymousUnionId());
        }
        spec->typenameView = symTab->AddSymbolName(anonName.c_str());
    }
   
    SymbolType* sym = symTab->QueryTypeSymbol(spec->typenameView);
    if(!sym)
    {
        // declare name
        BuiltIn::Type symType =  isStruct ? BuiltIn::struct_t :  BuiltIn::union_t;
        symTab->AddSymbol<SymbolType>(spec->typenameView, symType, 0, nullptr, nullptr, nullptr, false);
    }
    if(!structTree->rChild)
    {
        // test whether its declaration of type or object of said type
        return;
    }

    sym = symTab->QueryTypeSymbol(spec->typenameView);
    if(sym->isDefined)
    {
        // used simply to trigger redefinition error
        symTab->AddSymbol<SymbolType>(spec->typenameView, BuiltIn::struct_t, 0, nullptr, nullptr, nullptr, false);
    }
    // struct has its own scope
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

    std::string_view* argNames = symTab->AllocateTypeArrayOnHeap<std::string_view>(argCount);
    Member* members = symTab->AllocateTypeArrayOnHeap<Member>(argCount);
    size_t idx = 0;
    for(size_t i = 0; i < structDecls.size(); i++)
    {
        for(size_t j = 0; j < structDecls[i].declarators.size(); j++)
        {
            SymbolType* memType = symTab->QueryTypeSymbol(structDecls[i].declSpec.typenameView);

            members[idx].declType = structDecls[i].declSpec.declType;
            members[idx].typeName = structDecls[i].declSpec.typenameView;
            members[idx].bitCount = structDecls[i].declarators[j].bitCount;
            members[idx].access = structDecls[i].declarators[j].decl.accessTypes;
            members[idx].memberType = memType->dType;
            argNames[idx] = structDecls[i].declarators[j].decl.name;

            if(members[idx].bitCount != -1 && 
                (members[idx].access.type != NONE ||  
                 members[idx].memberType < BuiltIn::s_char_8 ||
                 members[idx].memberType > BuiltIn::u_int_64 ))
            {
                printf("Bitfield can only be used with integer types \n");
                exit(-1);
            }

            if(members[idx].bitCount != -1)
            {
                if(members[idx].bitCount > BuiltInBitCount( members[idx].memberType))
                {
                    printf("bitfield cannot be larger than original type\n");
                    exit(-1);
                }
                //unsigned types are divisible by 2
                members[idx].memberType = 
                    BitCountToIntegerType(members[idx].bitCount, members[idx].memberType%2);
            }

            idx++;
        }
    }

    sym->structTable = scopedTable;
    sym->argCount = argCount;
    sym->memberNames = argNames;
    sym->memberList = members;
    sym->isDefined = true;
    return; 
}

void SemanticAnalyzer::AnalyzeInitDeclList(DeclSpecs *declSpec, const Ast::Node *initDeclList)
{
    const Ast::Node* parent = initDeclList;
    SymbolType* symType = symTab->QueryTypeSymbol(declSpec->typenameView);
    codeGen.EmitUnionStruct(symType, declSpec->typenameView);

    while (Ast::Node* listElem = parent->rChild)
    {
        const Ast::Node* initDecl = listElem->lChild;
        const Ast::Node* initExpr = listElem->rChild;

        Declarator decl = AnalyzeDeclarator(initDecl->rChild);


        parent = listElem;
    }
    
}

Declarator SemanticAnalyzer::AnalyzeDeclarator(const Ast::Node *declarator)
{
    static std::stack<const Ast::Node*> accessTypes;
    Declarator decl = {};
    
    const Ast::Node *currDecl = declarator;
    while (true)
    {
        if(currDecl->rChild) {accessTypes.push(currDecl->rChild);}
        if(!currDecl->lChild)
        {
            break;
        }
        currDecl = currDecl->lChild;
    }
    
    decl.name = GetViewForToken(currDecl->token);
    AccessType* typePtr = &decl.accessTypes;
    uint32_t level = 0;
    while (accessTypes.size() > 0)
    {
        const Ast::Node* accessType = accessTypes.top();
        accessTypes.pop();
        while (accessType)
        {
            typePtr->level = level;
            if(accessType->type == Ast::pointer)
            {
                typePtr->type = POINTER;
                typePtr->ptr.quals = AnalyzeDeclSpec(accessType->lChild).declType.qual;
                goto create_next;
            }

            if(accessType->lChild->type == Ast::array_decl)
            {
                typePtr->type = ARRAY;
                typePtr->array.asmExpr = accessType->lChild->rChild;
                typePtr->array.qualList = accessType->lChild->lChild;
            }
            else if (accessType->lChild->type == Ast::parameter_type_list)
            {
                typePtr->type = FN_DECL;
                typePtr->fnDecl.paramTypeList = accessType->lChild;
            }
            else if (accessType->lChild->type == Ast::identifier_list)
            {
                typePtr->type = FN_CALL;
                typePtr->fnCall.identifierList = accessType->lChild;
            }
            else
            {
                printf("Incorrect node type in AnalyzeDeclarator \n");
                exit(-1);
            }
create_next:
            accessType = accessType->rChild;
            if(accessTypes.size() > 0 || accessType )
            {
                typePtr->next = symTab->AllocateTypeOnHeap<AccessType>();
                typePtr = typePtr->next;
            }
        }
        level++;
    }
    
    return decl;
}

void SemanticAnalyzer::AnalyzeEnum(const Ast::Node *enumTree, DeclSpecs *spec)
{
    printf("Unsupported enum\n");
    exit(-1);
}

int SemanticAnalyzer::BuiltInBitCount(BuiltIn::Type type)
{
    switch (type)
    {
    case BuiltIn::bool_t:    return 8;
    case BuiltIn::s_char_8:  return 8; 
    case BuiltIn::u_char_8:  return 8; 
    case BuiltIn::s_int_16:  return 16;
    case BuiltIn::u_int_16:  return 16;
    case BuiltIn::s_int_32:  return 32;
    case BuiltIn::u_int_32:  return 32;
    case BuiltIn::s_int_64:  return 64;
    case BuiltIn::u_int_64:  return 64;
    case BuiltIn::float_32:  return 32;
    case BuiltIn::double_64:  return 64;
    case BuiltIn::long_double:  return 128;
    case BuiltIn::complex_float_64:  return 128;
    case BuiltIn::complex_double_128:  return 128;
    default:
        return -1;
    }

    return -1;
}

BuiltIn::Type SemanticAnalyzer::BitCountToIntegerType(uint8_t BitCount, bool isSigned)
{

    for(uint16_t type = BuiltIn::s_char_8; type <= BuiltIn::s_int_64; type += 2)
    {
        if(BuiltInBitCount((BuiltIn::Type)type) > BitCount)
        {
            return isSigned ? (BuiltIn::Type)type : (BuiltIn::Type)(type + 1);
        }
    }

    return isSigned ? BuiltIn::s_int_64 : BuiltIn::u_int_64;
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
        printf("'%s' does not name a type \n", handyString.c_str());
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

void SemanticAnalyzer::WriteCodeToFile(const char *filename)
{
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC,  
                  S_IRUSR | S_IWUSR |  S_IRGRP | S_IWGRP |  S_IROTH | S_IWOTH);  
    codeGen.WriteToFile(fd);
    close(fd);
}

bool SemanticAnalyzer::NamesAType(const std::string_view& identifier)
{
    return (symTab->QuerySymKinds(identifier) & (Sym::TYPEDEF | Sym::TYPE) ) > 0;
}

uint64_t SemanticAnalyzer::GetAnnonymousStructId()
{
    static uint64_t id = 0;
    return id++;
}

uint64_t SemanticAnalyzer::GetAnnonymousUnionId()
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
            const Ast::Node* qualNodes = currNode;
            do
            {
                switch (qualNodes->token.type)
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
            } while ((qualNodes = qualNodes->lChild));
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
            if( (symTab->QuerySymKinds(GetViewForToken(currNode->token)) & Sym::TYPEDEF)  > 0)
            {
                SymbolTypedef* symTypedef = symTab->QueryTypedefSymbol(GetViewForToken(currNode->token));
                
                spec.typenameView = symTypedef->refrencedType;
                currNode = currNode->rChild;
                continue;
            }   
            switch (currNode->token.type)
            {
            case TokenType::kw_union:
                AnalyzeStructUnion(currNode->lChild, &spec, false);
                break;
            case TokenType::kw_struct:
                AnalyzeStructUnion(currNode->lChild, &spec, true);
                break;
            case TokenType::kw_enum:
                AnalyzeEnum(currNode->lChild, &spec);
                break;
            default:
                AnalyzeSimpleType(currNode, &spec);
                break;
            }
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

        currNode = currNode->rChild;
        
    }

    return spec;
}
