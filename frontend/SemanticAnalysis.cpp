#include "SemanticAnalysis.hpp"
#include <string.h>
#include <limits>
#include <fcntl.h>
#include <unistd.h>
#include "../utils/DataEncoder.hpp"
#include "../utils/Logger.hpp"
#include "../utils/Misc.hpp"
#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

constexpr StructDesc emptyDesc = {NOT_EMITTED, 0, nullptr, nullptr, nullptr};
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
symTab(symTab), manager(manager), ne(manager, this), 
codeGen(symTab, manager, &ne), logger(manager, "Semantic Analysis"),
utilHeap(5)
{
    // each simple built in typename is stored in read section during program lifetime
    // so each std::string_view will be valid
    symTab->AddSymbol<SymbolType>(kTypeNames[0], BuiltIn::void_t, true, 0, 0, emptyDesc);

    // integer types
    symTab->AddSymbol<SymbolType>(kTypeNames[1], BuiltIn::s_char_8, true, 1, 1, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[2], BuiltIn::s_char_8, true, 1, 1, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[3], BuiltIn::u_char_8, true, 1, 1, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[4], BuiltIn::s_int_16, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[5], BuiltIn::s_int_16, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[6], BuiltIn::s_int_16, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[7], BuiltIn::s_int_16, true, 2, 2, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[8], BuiltIn::u_int_16, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[9], BuiltIn::u_int_16, true, 2, 2, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[10], BuiltIn::s_int_32, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[11], BuiltIn::s_int_32, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[12], BuiltIn::s_int_32, true, 4, 4, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[13], BuiltIn::u_int_32, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[14], BuiltIn::u_int_32, true, 4, 4, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[15], BuiltIn::s_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[16], BuiltIn::s_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[17], BuiltIn::s_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[18], BuiltIn::s_int_64, true, 8, 8, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[19], BuiltIn::u_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[20], BuiltIn::u_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[21], BuiltIn::u_int_64, true, 8, 8, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[22], BuiltIn::s_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[23], BuiltIn::s_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[24], BuiltIn::s_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[25], BuiltIn::s_int_64, true, 8, 8, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[26], BuiltIn::u_int_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[27], BuiltIn::u_int_64, true, 8, 8, emptyDesc);

    // floats
    symTab->AddSymbol<SymbolType>(kTypeNames[28], BuiltIn::float_32, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[29], BuiltIn::double_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[30], BuiltIn::long_double, true, 16, 16, emptyDesc);

    // rest
    symTab->AddSymbol<SymbolType>(kTypeNames[31], BuiltIn::u_char_8, true, 1, 1, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[32], BuiltIn::complex_float_64, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[33], BuiltIn::complex_double_128, true, 16, 16, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[34], BuiltIn::complex_long_double, true, 32, 16, emptyDesc);

    // special built-in
    symTab->AddSymbol<SymbolType>(kTypeNames[35], BuiltIn::special, true, 0, 0, emptyDesc);

}

void SemanticAnalyzer::Analyze(const Ast::Node *root)
{
    if(root->type == Ast::declaration)
    {
        AnalyzeDeclaration(root->lChild, root->rChild);
    }
    else if(root->type == Ast::function_def)
    {
        AnalyzeFunctionDef(root->lChild, root->rChild);
    }
    else if (root->type == Ast::expression)
    {
        const Ast::Node* expr = root->rChild;
        while (expr)
        {
            AnalyzeExpr(expr->lChild);
            expr = expr->rChild;
        }
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

void SemanticAnalyzer::AnalyzeFunctionDef(const Ast::Node *decl, const Ast::Node *body)
{
    DeclSpecs declSpec = AnalyzeDeclSpec(decl->lChild->rChild);
    //codeGen.EmitUnionStruct(symType, declSpec->typenameView);
    // extract required nodes from ast tree
    const Ast::Node* initList = decl->rChild;
    const Ast::Node* initDecl = initList->rChild->lChild;

    Declarator fnDecl = AnalyzeDeclarator(initDecl->rChild, nullptr, nullptr);
    SymbolFunction* fnSym = symTab->QueryFunctionSymbol(fnDecl.name);
    if(fnSym != nullptr)
    {
        IssueWarning(&fnDecl.token, "Function redefinition")
    }
    AnalyzeFunctionDecl(&declSpec, &fnDecl);
    

    if(!IsPointer(&fnDecl.accArr , 1))
    {
        codeGen.EmitUnionStruct(declSpec.symType, declSpec.typenameView);
    }

    codeGen.EmitFunctionName(&declSpec, &fnDecl);
    const Ast::Node* bodyNode = body->rChild;

    symTab->CreateNewScope(Scope::LOCAL);
    while (bodyNode)
    {
        Analyze(bodyNode->lChild);
        bodyNode = bodyNode->rChild;
    }
    symTab->PopScope();

    codeGen.EmitFunctionClose();
    
}

void SemanticAnalyzer::AnalyzeFunctionDecl(DeclSpecs *spec, Declarator *decl)
{
    AccessType* FnDecl = &decl->accArr.ptr[0];
    // on empty function parameter declaration add void type
    if(FnDecl->fnDecl.paramCount == 0)
    {
        FnDecl->fnDecl.paramCount = 1;
        FunctionParams* paramsPtr = symTab->AllocateTypeArrayOnHeap<FunctionParams>(1);
        paramsPtr->decl = {};
        paramsPtr->spec.typenameView = kTypeNames[0];
        paramsPtr->spec.declType = {};
        paramsPtr->spec.symType = symTab->QueryTypeSymbol(paramsPtr->spec.typenameView);
        FnDecl->fnDecl.paramTypeList = paramsPtr;
    }
    //const Ast::Node* args = FnDecl->fnDecl.paramTypeList;
    SymbolFunction* tableSym = symTab->QueryFunctionSymbol(decl->name);
    if(tableSym)
    {
        if(tableSym->paramCount != FnDecl->fnDecl.paramCount||
           !CompareParams(tableSym->paramCount, tableSym->params, FnDecl->fnDecl.paramTypeList) )
        {
            IssueWarning(&decl->token, "Conflicting function declarations")
        }
    }
    else
    {
        symTab->AddSymbol<SymbolFunction>(decl->name, FnDecl->fnDecl.paramCount, 0, FnDecl->fnDecl.paramTypeList, nullptr, false);
    }
}

Declarator SemanticAnalyzer::ProcessDecl(const Ast::Node *declarator, std::stack<const Ast::Node*>* accessTypes, bool isAbstract, const AccessArray* typedefAcc)
{
    Declarator decl = {};
    decl.token = declarator->token;
    if(!isAbstract)
    {
        decl.name = GetViewForToken(accessTypes->top()->token);
        accessTypes->pop();
    }

    AccessType typeBuffer = {};
    uint32_t level = 0;
    std::vector<AccessType> localTypes;
    localTypes.reserve(50);

    while (accessTypes->size() > 0)
    {
        const Ast::Node* accessType = accessTypes->top();
        accessTypes->pop();
        while (accessType)
        {
            typeBuffer.level = level;
            if(accessType->type == Ast::pointer)
            {
                typeBuffer.type = ACC_POINTER;
                typeBuffer.ptr.quals = AnalyzeDeclSpec(accessType->lChild).declType.qual;
                goto create_next;
            }

            if(accessType->lChild->type == Ast::array_decl)
            {
                Typed::Number num = ne.ExecuteNode(accessType->lChild->rChild);
                if(num.type == Typed::d_dynamic)
                {
                    typeBuffer.type = ACC_ARRAY_VLA;
                    typeBuffer.array.asmExpr = accessType->lChild->rChild;
                }
                else if (num.type == Typed::d_none)
                {
                    typeBuffer.type = ACC_ARRAY;
                    typeBuffer.array.size = CG_EMPTY_ARRAY;
                }
                else
                {
                    typeBuffer.type = ACC_ARRAY;
                    typeBuffer.array.size = Typed::CastTo<uint64_t>(num);
                    if(typeBuffer.array.size == 0 )
                    {
                        typeBuffer.array.size = CG_ZERO_SIZED_ARRAY;
                    }
                }
                
                typeBuffer.array.quals = AnalyzeDeclSpec(accessType->lChild->lChild).declType.qual;
            }
            else if (accessType->lChild->type == Ast::parameter_type_list)
            {
                typeBuffer.type = ACC_FN_DECL;
                typeBuffer.fnDecl.paramTypeList = ProcessFnParams(accessType->lChild, &typeBuffer.fnDecl.paramCount);
            }
            else if (accessType->lChild->type == Ast::identifier_list)
            {
                if(isAbstract) 
                {
                    IssueWarning(&accessType->token, "Function call is not allowed in abstract declarator" )
                }
                typeBuffer.type = ACC_FN_CALL;
                typeBuffer.fnCall.identifierList = accessType->lChild;
            }
            else
            {
                IssueWarning(&accessType->token, "Incorrect node type in AnalyzeDeclarator" )
            }
create_next:
            accessType = accessType->rChild;
            localTypes.push_back(typeBuffer);
        }
        level++;
    }
    
    decl.accArr.count = localTypes.size();
    if(typedefAcc)
    {
        decl.accArr.count += typedefAcc->count;
    }
    decl.accArr.ptr = symTab->AllocateTypeArrayOnHeap<AccessType>(decl.accArr.count);
    memcpy(decl.accArr.ptr, localTypes.data(), sizeof(AccessType) * localTypes.size());

    if(typedefAcc)
    {
        memcpy(decl.accArr.ptr + localTypes.size(), typedefAcc->ptr, sizeof(AccessType) * typedefAcc->count);
    }
    return decl;
}

FunctionParams *SemanticAnalyzer::ProcessFnParams(const Ast::Node *paramsNode, size_t* paramCount)
{
    std::vector<FunctionParams> params;
    
    const Ast::Node* glueNode = paramsNode->rChild;
    while (glueNode)
    {
        const Ast::Node* paramNode = glueNode->lChild;

        FunctionParams param = {};
        if(glueNode->type == Ast::parameter_decl)
        {
            if(glueNode->rChild) { IssueWarning(&glueNode->token, "elipsis MUST be the last parameter")}
            param.spec.declType.isEllipsis = 1;
            params.push_back(param);
            break;
        }
        else
        {
            param.spec = AnalyzeDeclSpec(paramNode->lChild->rChild);
            // TODO validate whethere decl does not contain FN_CALL
            param.decl = AnalyzeDeclarator(paramNode->rChild, param.spec.accArr, nullptr); 
            params.push_back(param);
        }
        glueNode = glueNode->rChild;
    }
    if(paramCount)
    {
        *paramCount = params.size();
    }
    
    if(params.size() == 0)
    {
        return nullptr;
    }

    FunctionParams* paramsPtr = symTab->AllocateTypeArrayOnHeap<FunctionParams>(params.size());
    memcpy(paramsPtr, params.data(), sizeof(FunctionParams) * params.size() );

    params.clear();
    return paramsPtr;
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
        initDecl.decl = AnalyzeDeclarator(structDeclarator->lChild, structDecl.declSpec.accArr, nullptr);
        int64_t bitCount = -1;
        if(structDeclarator->rChild)
        {
            Typed::Number constExpr;
            memcpy(&constExpr, structDeclarator->rChild->lChild, sizeof(Typed::Number));
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
        Declarator decl = AnalyzeDeclarator(initDecl->rChild, declSpec->accArr,  initDecl->lChild);

        if(decl.initExpr)
        {
            printf("typedef is not allowed to have initializer\n");
            exit(-1);
        }
        if(IsPointer(&decl.accArr))
        {
            PointerDesc ptrDesc;
            ptrDesc.accessTypes = decl.accArr;
            ptrDesc.spec = *declSpec;
            symTab->AddSymbol<SymbolType>(decl.name, BuiltIn::ptr, true, 8, 8, ptrDesc);
        }
        else
        {
            symTab->AddSymbol<SymbolTypedef>(decl.name, declSpec->typenameView, declSpec->declType.qual, decl.accArr);
        }
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
        symTab->AddSymbol<SymbolType>(spec->typenameView, symType, false, 0, 0, emptyDesc );
    }

    spec->symType = symTab->QueryTypeSymbol(spec->typenameView);
    if(!structTree->rChild)
    {
        // test whether its declaration of type or object of said type
        return;
    }

    sym = symTab->QueryTypeSymbol(spec->typenameView);
    if(sym->isDefined)
    {
        // used simply to trigger redefinition error
        symTab->AddSymbol<SymbolType>(spec->typenameView, BuiltIn::struct_t, false, 0, 0, emptyDesc);
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
    //ScopedSymbolTable* scopedTable = symTab->currentTable;

    std::string_view* argNames = symTab->AllocateTypeArrayOnHeap<std::string_view>(argCount);
    Member* members = symTab->AllocateTypeArrayOnHeap<Member>(argCount);
    size_t idx = 0;
    uint32_t highestAlignment = 1;
    uint64_t structSize = 0;
    for(size_t i = 0; i < structDecls.size(); i++)
    {
        for(size_t j = 0; j < structDecls[i].declarators.size(); j++)
        {
            SymbolType* memType = structDecls[i].declSpec.symType;
            if(!memType)
            {
                IssueWarning(&structDecls[i].declarators[j].decl.token, "struct member: unknown base type from declaration specifiers");
            }
            argNames[idx] = structDecls[i].declarators[j].decl.name;

            members[idx].typedefAccArr = structDecls[i].declSpec.accArr ? *structDecls[i].declSpec.accArr : AccessArray{};
            members[idx].declType = structDecls[i].declSpec.declType;
            members[idx].typeName = structDecls[i].declSpec.typenameView;
            members[idx].bitCount = structDecls[i].declarators[j].bitCount;
            members[idx].accArr = structDecls[i].declarators[j].decl.accArr;
            members[idx].memberType = memType->dType;
            // memory related processing
            
            MemoryDesc desc = GetMemoryDesc(&members[idx].accArr, memType, &logger, &ne);
            members[idx].size = desc.size;
            members[idx].alignment = desc.alignment;
            // post processing related to size and offsets within struct/union itself
            highestAlignment = std::max(highestAlignment, members[idx].alignment);
            size_t aling = structSize % members[idx].alignment;
            structSize += aling ? members[idx].alignment - aling : 0;
            structSize += members[idx].size;

            SymbolType* memberSymType = symTab->QueryTypeSymbol(members[idx].typeName);
            if( !IsMemberPointer(&members[idx]) && (!memberSymType || !memberSymType->isDefined))
            {
                IssueWarning(&structDecls[i].declarators[j].decl.token, "struct member type is not defined");
            }

            if(members[idx].bitCount != -1 && 
                (members[idx].accArr.count > 0 ||  
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
                // for now bitfields HAVE NO effect on the llvm code ie they are disabled

                //unsigned types are divisible by 2
                //members[idx].memberType = 
                //    BitCountToIntegerType(members[idx].bitCount, members[idx].memberType%2);
                //members[idx].typeName = SimpleTypeToString(members[idx].memberType);
            }

            idx++;
        }
    }

    //sym->str.structTable = scopedTable;
    sym->str.argCount = argCount;
    sym->str.memberNames = argNames;
    sym->str.memberList = members;
    sym->isDefined = true;
    sym->alignment = highestAlignment;
    sym->size = structSize;
    return; 
}

void SemanticAnalyzer::AnalyzeInitDeclList(DeclSpecs *declSpec, const Ast::Node *initDeclList)
{
    const Ast::Node* parent = initDeclList;
    SymbolType* symType = declSpec->symType;
    codeGen.EmitUnionStruct(symType, declSpec->typenameView);

    while (Ast::Node* listElem = parent->rChild)
    {
        const Ast::Node* initDecl = listElem->lChild;

        Declarator decl = AnalyzeDeclarator(initDecl->rChild, declSpec->accArr, initDecl->lChild);

        if(decl.accArr.count > 0 && decl.accArr.ptr[0].type == ACC_FN_DECL)
        {
            if(symTab->currentTable->scopeType != Scope::GLOBAL)
            {
                IssueWarning(&decl.token, "Function definitions in local scope are forbidden");
            }
            AnalyzeFunctionDecl(declSpec, &decl);
        }
        else
        {
            AnalyzeVariableDecl(declSpec, &decl);
        }

        parent = listElem;
    }
    
}

bool SemanticAnalyzer::CompareParams(size_t paramCount, const FunctionParams *p1, const FunctionParams *p2)
{
    for(size_t i = 0; i < paramCount; i++)
    {
        if(!CompareDeclSpec(&p1[i].spec, &p2[i].spec) || 
           !CompareDeclarators(&p1[i].decl, &p2[i].decl))
        {
            return false;
        }
    }


    return true;
}

Declarator SemanticAnalyzer::AnalyzeDeclarator(const Ast::Node *declarator, const AccessArray* typedefAcc, const Ast::Node *initExpr)
{
    std::stack<const Ast::Node*> accessTypes;
    const Ast::Node *currDecl = declarator;
    while (true)
    {
        if(currDecl->rChild) {accessTypes.push(currDecl->rChild);}
        if(!currDecl->lChild)
        {
            if(currDecl->type == Ast::direct_declarator) {accessTypes.push(currDecl);}
            break;
        }
        currDecl = currDecl->lChild;
    }

    Declarator decl = {};
    if(declarator->type == Ast::abstact_declarator)
    {
       decl = ProcessDecl(declarator, &accessTypes, true, typedefAcc);
    }
    else
    {
        decl = ProcessDecl(declarator, &accessTypes, false, typedefAcc);
    }
    decl.initExpr = initExpr;
    if(decl.initExpr)
    {
        DeduceInferableArrSize(&decl);
    }

    return decl;

}

void SemanticAnalyzer::AnalyzeEnum(const Ast::Node *enumTree, DeclSpecs *spec)
{
    if(enumTree->token.type == TokenType::identifier)
    {
        // reserve name for non anonymous enum
        symTab->AddSymbol<SymbolType>(GetViewForToken(enumTree->token), BuiltIn::enum_t, true, 4, 4, emptyDesc);
    }
    // enums are to be treated as i32 by type system
    spec->symType = symTab->QueryTypeSymbol(std::string_view(kTypeNames[12]));
    spec->typenameView = std::string_view(kTypeNames[12]);
    spec->declType = {};


    int64_t value = 0;
    const Ast::Node* enumerator = enumTree->rChild;
    while (enumerator)
    {
        if(enumerator->lChild)
        {
            Typed::Number num = ne.ExecuteNode(enumerator->lChild);
            switch (num.type)
            {
            case Typed::DType::d_int8_t:  value = num.int8; break;
            case Typed::DType::d_int16_t: value = num.int16; break;
            case Typed::DType::d_int32_t: value = num.int32; break;
            case Typed::DType::d_int64_t: value = num.int64; break;
            default: IssueWarning(&enumerator->token, "Non integer value is not allowed enumerator");
            }
        }
        VariableOpts opts = {.isEnumerator = 1, .isConst = 0};
        std::string_view enumName = GetViewForToken(enumerator->token);
        Declarator decl = {enumerator->token, {}, enumName, nullptr};
        symTab->AddSymbol<SymbolVariable>(enumName, symTab->currentTable->scopeType, spec, &decl, &opts, value);
        
        value++;
        enumerator = enumerator->rChild;
    }
    
}

void SemanticAnalyzer::DeduceInferableArrSize(Declarator *decl)
{
    const AccessArray* arr = &decl->accArr;
    if(arr->count == 0)
    {
        return;
    }

    if(!(arr->ptr[0].type == ACC_ARRAY && arr->ptr[0].array.size == CG_EMPTY_ARRAY))
    {
        return;
    }

    const AccessArray nextAcc = {decl->accArr.ptr + 1, decl->accArr.count - 1};
    std::vector<ArrayInitPair> pairs = PartitionArrayInitializer(decl->initExpr, &nextAcc, &logger, &ne, nullptr);

    //pairs.back().idx stores largest valid index into array so lenght is +1
    arr->ptr[0].array.size = pairs.size() > 0 ? pairs.back().idx + 1 : CG_ZERO_SIZED_ARRAY;
}

int SemanticAnalyzer::BuiltInBitCount(BuiltIn::Type type)
{
    switch (type)
    {
    case BuiltIn::bool_t:              return 8;
    case BuiltIn::s_char_8:            return 8; 
    case BuiltIn::u_char_8:            return 8; 
    case BuiltIn::s_int_16:            return 16;
    case BuiltIn::u_int_16:            return 16;
    case BuiltIn::s_int_32:            return 32;
    case BuiltIn::u_int_32:            return 32;
    case BuiltIn::s_int_64:            return 64;
    case BuiltIn::u_int_64:            return 64;
    case BuiltIn::float_32:            return 32;
    case BuiltIn::double_64:           return 64;
    case BuiltIn::long_double:         return 128;
    case BuiltIn::complex_float_64:    return 128;
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
    static std::string handyString;

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

bool SemanticAnalyzer::CompareDeclSpec(const DeclSpecs *s1, const DeclSpecs *s2)
{
    return s1->declType.storageFlags == s2->declType.storageFlags &&
           s1->declType.qualifierFlags == s2->declType.qualifierFlags &&
           s1->declType.inlineSpec == s2->declType.inlineSpec &&
           s1->declType.isEllipsis == s2->declType.isEllipsis &&
           s1->typenameView == s2->typenameView;
}

bool SemanticAnalyzer::CompareDeclarators(const Declarator *d1, const Declarator *d2)
{
    if(d1->accArr.count == 0 &&
       d2->accArr.count == 0)
    {
        return true;
    }

    if(d1->accArr.count != d2->accArr.count)
    {
        return false;
    }


    for(size_t i = 0; i < d1->accArr.count; i ++)
    {
        const AccessType* acc1 = &d1->accArr.ptr[i];
        const AccessType* acc2 = &d2->accArr.ptr[i];

        if(acc1->type != acc2->type)
        {
            return false;
        }

        if(acc1->type == ACC_POINTER &&
           acc1->ptr.quals != acc2->ptr.quals )
        {
            return false;
        }
        else if(acc1->type == ACC_FN_DECL && 
                (acc1->fnDecl.paramCount != acc2->fnDecl.paramCount || 
                 CompareParams(acc1->fnDecl.paramCount, acc1->fnDecl.paramTypeList, acc2->fnDecl.paramTypeList)) )
        {
            return false;
        }
        else if(acc1->type == ACC_FN_CALL)
        {
            IssueWarning(nullptr, "Internal error, ACC_FN_CALL is not supported in CompareDeclarators");
            return false;
        }
        else if(acc1->type == ACC_ARRAY)
        {
            Typed::Number num1 = ne.ExecuteNode(acc1->array.asmExpr);
            Typed::Number num2 = ne.ExecuteNode(acc2->array.asmExpr);
            if(num1 != num2)
            {
                return false;
            }
        }

    }
    
    return true;
}

void SemanticAnalyzer::EmitUninitializedGlobals()
{
    for(SymbolVariable* symVar : uninitGlobals)
    {
        symVar->opts.isEmitted = 1;
        codeGen.EmitGlobalVariable(&symVar->spec, &symVar->decl);
        InitGlobalVar(&symVar->spec, &symVar->decl);
    }
}

void SemanticAnalyzer::ResolveIntegralPromotion(ExprRet *left, ExprRet *right, BuiltIn::Type* outLeft, BuiltIn::Type* outRight)
{
    if(isSmallInteger(left->type) && (isSmallInteger(right->type)))
    {
        *outLeft = BuiltIn::s_int_32;
        *outRight = BuiltIn::s_int_32;
        return;
    }

    if(left->type == right->type)
    {
        *outLeft = left->type;
        *outRight= right->type;
        return;
    }

    int rankLeft = GetIntRank(left->type);
    int rankRight = GetIntRank(right->type);
    int maxRank = std::max(rankLeft, rankRight);
    // signed values are even and unsigned are odd
    if(left->type %2 == right->type%2)
    {
        if(rankLeft > rankRight)
        {
            *outLeft = left->type;
            *outRight= left->type;
        }
        else
        {
            *outLeft = right->type;
            *outRight= right->type;
        }
        return;
    }
    // here we know that we have mixed types
    const ExprRet* unsignedType = isUnsigned(left->type) ? left : nullptr;
    unsignedType = unsignedType == nullptr ? right : unsignedType;
    const ExprRet* signedType = isSigned(left->type) ? left : nullptr;
    signedType = signedType == nullptr? right : signedType;

    if(GetIntRank(unsignedType->type) == maxRank)
    {
        *outLeft = unsignedType->type;
        *outRight= unsignedType->type;
        return;
    }
    else
    {
        *outLeft = signedType->type;
        *outRight= signedType->type;
        return;
    }

}

int SemanticAnalyzer::GetIntRank(BuiltIn::Type type)
{
    static const int intRanks[] = {1, 1, 2, 2, 3, 3, 4, 4}; // i8, ui8, i16, ui16, i32, ui32, i64, ui64
    if(type < BuiltIn::s_char_8 || type > BuiltIn::u_int_64)
    {
        return -1;
    }

    return  intRanks[type - BuiltIn::s_char_8];
}

bool SemanticAnalyzer::NamesAType(const std::string_view& identifier)
{
    return (symTab->QuerySymKinds(identifier) & (Sym::TYPEDEF | Sym::TYPE) ) > 0;
}

void SemanticAnalyzer::AnalyzeVariableDecl(const DeclSpecs* spec, const Declarator* decl)
{
    if(symTab->IsCurrentScopeGlobal() || spec->declType.spec.static_)
    {
        AnalyzeGlobalVarDecl(spec, decl);
    }
    else
    {
        AnalyzeLocalVarDecl(spec, decl);
    }
}

void SemanticAnalyzer::AnalyzeGlobalVarDecl(const DeclSpecs* spec, const Declarator* decl)
{
    SymbolVariable* symVar = symTab->QueryVarSymbol(decl->name);

    if(symVar == nullptr)
    {
        VariableOpts opts = {.isEnumerator = 0, .isConst = 0, .isEmitted = 0};
        symTab->AddSymbol<SymbolVariable>(decl->name, symTab->currentTable->scopeType, spec, decl, &opts);
        symVar = symTab->QueryVarSymbol(decl->name);
    }

    if(!decl->initExpr)
    {
        if(symVar->opts.isEmitted == 0 && uninitGlobals.find(symVar) == uninitGlobals.end())
        {
            uninitGlobals.insert(symVar);
        }
    }
    else
    {
        if(symVar->opts.isEmitted)
        {
            int len = (int) decl->name.length();
            IssueWarning(nullptr, "Symbol \'%.*s\' redefinition", len, decl->name.data());
        }
        symVar->opts.isEmitted = 1;
        codeGen.EmitGlobalVariable(spec, decl);
        InitGlobalVar(spec, decl);

        if(uninitGlobals.find(symVar) != uninitGlobals.end())
        {
            uninitGlobals.erase(symVar);
        }
    }
}

void SemanticAnalyzer::InitGlobalVar(const DeclSpecs *spec, const Declarator *decl)
{
    if(!decl->initExpr)
    {
        codeGen.ZeroInitGlobalVar(spec, decl);
        return;
    }

    if(!IsPointer(&decl->accArr) && (spec->symType->dType == BuiltIn::struct_t || spec->symType->dType == BuiltIn::union_t ))
    {
        IssueWarning(nullptr, "Struct/union initialization is not supported")
    }

    if(decl->accArr.count == 0 || (decl->accArr.count > 0 && decl->accArr.ptr[0].type == ACC_POINTER))
    {
        codeGen.EmitGlobalBuiltInInit(decl->initExpr, spec->symType->alignment);
        return;
    }
    // array stuff
    InitGlobalArray(&decl->accArr, decl->initExpr, spec);

    codeGen.EmitGLobalArrayAlignment(IsPointer(&decl->accArr), spec->symType->alignment);
}

void SemanticAnalyzer::InitGlobalArray(const AccessArray *accArr, const Ast::Node *initExpr, const DeclSpecs *spec)
{
    if(spec->symType->dType == BuiltIn::struct_t || spec->symType->dType == BuiltIn::union_t )
    {
        IssueWarning(nullptr, "Array of non built-in types are not supported")
    }

    // init arrays
    const AccessArray nextAcc = {accArr->ptr + 1, accArr->count - 1};
    bool isNestedArray = nextAcc.count > 0 && IsArray(&nextAcc);

     // here we emit type
    if(accArr->count == 0 || (accArr->ptr[0].type == ACC_POINTER))
    {
        codeGen.EmitInitializer(spec, initExpr->lChild->lChild, false);
        return;
    }

    std::vector<ArrayInitPair> pairs;
    if(initExpr)
    {   
        pairs = PartitionArrayInitializer(initExpr, &nextAcc, &logger, &ne, &utilHeap);
    }

    uint64_t currentPair = 0;
    uint64_t arraySize = accArr->ptr->array.size;
    
    codeGen.StartArray();
    for(uint64_t i = 0; i < arraySize; i++)
    {

        codeGen.EmitDeclaratorAcc(&nextAcc, &spec->typenameView);
        if(currentPair < pairs.size() && i == pairs[currentPair].idx)
        {
            InitGlobalArray(&nextAcc, pairs[currentPair].initializerList, spec);
            currentPair++;
        }
        else
        {
            codeGen.EmitInitializer(spec, nullptr, isNestedArray);
        }

        if(i < arraySize - 1)
        {
            codeGen.ArgSeparator();
        }
    }
    codeGen.EndArray();
}

void SemanticAnalyzer::AnalyzeLocalVarDecl(const DeclSpecs *spec, const Declarator *decl)
{
    SymbolVariable* symVar = symTab->QueryVarSymbol(decl->name);

    if(symVar == nullptr)
    {
        VariableOpts opts = {.isEnumerator = 0, .isConst = 0, .isEmitted = 0};
        symTab->AddSymbol<SymbolVariable>(decl->name, symTab->currentTable->scopeType, spec, decl, &opts, codeGen.GetIdxForLocalVar());
        symVar = symTab->QueryVarSymbol(decl->name);
    }
    else
    {
        IssueWarning(&decl->token, "Local variable redefinition")
    }

    symVar->opts.isEmitted = 1;
    codeGen.EmitLocalVariable(symVar);
    InitLocalVariable(symVar);
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

bool SemanticAnalyzer::IsMemberPointer(const Member *member)
{
    if(member->accArr.count > 0)
    {
        for(size_t i = 0; i < member->accArr.count; i++)
        {
            if(member->accArr.ptr[i].type == ACC_POINTER)
            {
                return true;
            }
            else if(member->accArr.ptr[i].type == ACC_FN_CALL || 
                    member->accArr.ptr[i].type == ACC_FN_DECL)
            {
                return false;
            }
        }

    }
    return false;
}

std::string_view SemanticAnalyzer::SimpleTypeToString(BuiltIn::Type type)
{
     switch (type)
    {
    case BuiltIn::bool_t:               return kTypeNames[33];
    case BuiltIn::s_char_8:             return kTypeNames[2]; 
    case BuiltIn::u_char_8:             return kTypeNames[3]; 
    case BuiltIn::s_int_16:             return kTypeNames[5]; 
    case BuiltIn::u_int_16:             return kTypeNames[8]; 
    case BuiltIn::s_int_32:             return kTypeNames[10];
    case BuiltIn::u_int_32:             return kTypeNames[13];
    case BuiltIn::s_int_64:             return kTypeNames[21];
    case BuiltIn::u_int_64:             return kTypeNames[24];
    case BuiltIn::float_32:             return kTypeNames[29];
    case BuiltIn::double_64:            return kTypeNames[30];
    case BuiltIn::long_double:          return kTypeNames[31];
    case BuiltIn::complex_float_64:     return kTypeNames[34];
    case BuiltIn::complex_double_128:   return kTypeNames[35];
    default:
        printf("Chosen type is not simple built in type\n");
        exit(-1);
    }

    return "";
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
                if(!symTypedef)
                {
                    IssueWarning(&currNode->token, "typedef name did not resolve to a typedef symbol");
                }
                spec.typenameView = symTypedef->refrencedType;
                spec.symType = symTab->QueryTypeSymbol(symTypedef->refrencedType);
                spec.accArr = &symTypedef->accArr;
                if(!spec.symType)
                {
                    IssueWarning(&currNode->token, "typedef refers to unknown type");
                }
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
            case TokenType::identifier:
                // data that std::string_view encapsulates has lifetime of file manager.
                // File manager is kept alive over whole duration of program so 
                // it is safe to fetch typename from token
                spec.typenameView = GetViewForToken(currNode->token);
                spec.symType = symTab->QueryTypeSymbol(spec.typenameView);
                if(spec.symType->dType != BuiltIn::ptr)
                {
                    IssueWarning(nullptr, "Internal: spec.symType->dType != BuiltIn::ptr")
                }
                spec.accArr = &spec.symType->ptr.accessTypes;
                
                
                if(!spec.symType)
                {
                    IssueWarning(&currNode->token, "unknown type name");
                }
                break;
            default:
                AnalyzeSimpleType(currNode, &spec);
                spec.symType = symTab->QueryTypeSymbol(spec.typenameView);
                if(!spec.symType)
                {
                    IssueWarning(&currNode->token, "unknown type name");
                }
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
        if(!currNode->rChild)
        {
            break;
        }
        else
        {
            currNode = currNode->rChild;
        }
        
    }

    return spec;
}

void SemanticAnalyzer::InitLocalVariable(const SymbolVariable* symVar)
{
    if(!symVar->decl.initExpr)
    {
        return;
    }
    Ast::Node node = {
        .type = Ast::init_expr,
        .lChild = const_cast<Ast::Node*>(symVar->decl.initExpr)
    };
    ExprRet initInfo;
    initInfo.type = IsPointer(&symVar->decl.accArr) ? BuiltIn::ptr : symVar->spec.symType->dType; 
    initInfo.id = symVar->varIdx;
    node.rChild = (Ast::Node*)&initInfo;
    if(!IsArray(&symVar->decl.accArr))
    {
        AnalyzeExpr(&node);
    }
    else
    {
        //codeGen.InitLocalArray(symVar->decl.name, &symVar->decl.accArr, symVar->decl.initExpr, &symVar->spec);
    }
}

ExprRet SemanticAnalyzer::AnalyzeExpr(const Ast::Node *root)
{
    switch (root->type)
    {
    case Ast::get_addr:
    {
        ExprRet handle = AnalyzeExpr(root->lChild);
        if(handle.id == EXPR_ID_IGNORE)
        {
            return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
        }
        return {BuiltIn::ptr, {}, handle.id};
        
    }break;
    case Ast::assignment: return HandleAssignment(root);
    case Ast::identifier: return HandleIdentifier(root);
    case Ast::op_add: return HandleAddition(root);
    case Ast::op_minus: return HandleOpMinus(root);
    case Ast::init_expr: return HandleInitExpr(root);        
    case Ast::character: return LoadCharacter(root);
    case Ast::constant: return LoadConstant(root);            
    case Ast::compound_literal: return CompoundLiteral(root); 
    default: return ExprRet{BuiltIn::none, {}, -1000};  break;
    }

    return ExprRet{BuiltIn::none, {}, -1000};
}

ExprRet SemanticAnalyzer::CompoundLiteral(const Ast::Node *literal)
{
    DeclSpecs spec = AnalyzeDeclSpec(literal->rChild->rChild);
    Declarator decl = {};
    if(literal->rChild->rChild)
    {
        decl = AnalyzeDeclarator(literal->rChild->lChild, spec.accArr, literal->lChild);
    }
    int64_t varIdx = codeGen.GetIdxForLocalVar();
    std::string tmpName = "compound_literal_" + std::to_string(varIdx);
    if(decl.name == "")
    {
        decl.name = tmpName;
    }
    VariableOpts opts = {.isEnumerator = 0, .isConst = 0, .isEmitted = 0};
    SymbolVariable localVar(Sym::Kind::VAR, symTab->currentTable->scopeType, &spec, &decl, &opts, varIdx);
    codeGen.EmitLocalVariable(&localVar);
    InitLocalVariable(&localVar);

    return {BuiltIn::struct_t, {}, localVar.varIdx};
}

ExprRet SemanticAnalyzer::LoadCharacter(const Ast::Node *constant)
{
    std::string_view str = GetViewForToken(constant->token);
    Typed::Number num;
    num.type = Typed::d_int8_t;
    num.int8 = str[1];
    return ExprRet{BuiltIn::none, num, EXPR_ID_CONST};
}

ExprRet SemanticAnalyzer::LoadConstant(const Ast::Node *constant)
{
    // system here is bit simplified as only 32 and 64 bit values in
    //  linux style convention are considered
    Typed::Number num;
    std::string_view str = GetViewForToken(constant->token);
    if(constant->token.isFloat)
    {
        long double value = stringToLongDouble(str.data(), str.length(), MODE_DEC);
        if(constant->token.f)
        {
            num.type = Typed::d_float;
            num.float32 = (float)value;
        }
        else if(constant->token.l)
        {
            num.type = Typed::d_l_double;
            num.lFloat = (long double)value;
        }
        else
        {
            num.type = Typed::d_double;
            num.float64 = (double)value;
        }
    }
    else if(constant->token.u)
    {
        uint64_t value = stringToUint64(str.data(), str.length(), MODE_DEC); 
        if(constant->token.l == 0 && value <= UINT32_MAX)
        {
            num.type = Typed::d_uint32_t;
            num.uint32 = (uint32_t)value;
        }
        else
        {
            num.type = Typed::d_uint64_t;
            num.uint64 = (uint64_t)value;
        }
    }
    else
    {
        int64_t value = stringToInt64(str, MODE_DEC);

        if (constant->token.l == 0 &&
            value >= INT32_MIN &&
            value <= INT32_MAX)
        {
            num.type = Typed::d_int32_t;
            num.int32 = static_cast<int32_t>(value);
        }
        else
        {
            num.type = Typed::d_int64_t;
            num.int64 = value;
        }
    }

    return ExprRet{BuiltIn::none, num, EXPR_ID_CONST};
}

ExprRet SemanticAnalyzer::HandleInitExpr(const Ast::Node *root)
{
    ExprRet* destHandle = (ExprRet*)root->rChild;
    ExprRet source = AnalyzeExpr(root->lChild);
    if(source.id == EXPR_ID_IGNORE)
    {
        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
    }

    if(destHandle->type == BuiltIn::struct_t ||
        destHandle->type == BuiltIn::union_t)
    {
        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
    }
    uint32_t alignment = GetBuiltInAlignemnt(destHandle->type);
    if(source.id == EXPR_ID_CONST)
    {
        codeGen.EmitLocalConstAsm(destHandle->type, alignment, destHandle->id, source.num);
    }
    else
    {
        codeGen.EmitLocalStorage(destHandle->type, alignment, destHandle->id, source.id);
    }
    return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
}

ExprRet SemanticAnalyzer::HandleOpMinus(const Ast::Node *root)
{
    ExprRet expr = AnalyzeExpr(root->lChild);
    if (expr.id == EXPR_ID_CONST)
    {
        Typed::Number neg;
        neg.type = expr.num.type;
        switch (neg.type)
        {
            case Typed::d_int8_t:   neg.int8   = -1; break;
            case Typed::d_int16_t:  neg.int16  = -1; break;
            case Typed::d_int32_t:  neg.int32  = -1; break;
            case Typed::d_int64_t:  neg.int64  = -1; break;
            case Typed::d_uint8_t:  neg.uint8  = -1; break;
            case Typed::d_uint16_t: neg.uint16 = -1; break;
            case Typed::d_uint32_t: neg.uint32 = -1; break;
            case Typed::d_uint64_t: neg.uint64 = -1; break;
            case Typed::d_float:    neg.float32 = -1; break;
            case Typed::d_double:   neg.float64 = -1; break;
            case Typed::d_l_double: neg.lFloat  = -1;  break;
            default: break;
        }
        expr.num = Typed::TypedBinOp<std::multiplies>(neg, expr.num);
        return expr;
    }
    
    return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
}

ExprRet SemanticAnalyzer::HandleAssignment(const Ast::Node *root)
{
    ExprRet exprRes = AnalyzeExpr(root->rChild);
    return ExprRet();
}

ExprRet SemanticAnalyzer::HandleAddition(const Ast::Node *root)
{
    ExprRet left = AnalyzeExpr(root->lChild);
    ExprRet right = AnalyzeExpr(root->rChild);
    ExprRet newLeft, newRight;
    HandleTypePromotion(&left, &right, &newLeft, &newRight);

    return ExprRet();
}

ExprRet SemanticAnalyzer::HandleIdentifier(const Ast::Node *root)
{
    std::string_view varName = GetViewForToken(root->token);
    const SymbolVariable* symVar = symTab->QueryVarSymbol(varName);
    ExprRet out = {};
    out.type = symVar->spec.symType->dType;
    out.id = codeGen.EmitLocalLoad(symVar->spec.symType->dType, symVar->spec.symType->alignment, symVar->varIdx);

    return out;
}

void SemanticAnalyzer::HandleTypePromotion(ExprRet *left, ExprRet *right, ExprRet *outLeft, ExprRet *outRight)
{
    if(isInteger(left->type) && isInteger(right->type))
    {
        BuiltIn::Type newLeft, newRight;
        ResolveIntegralPromotion(left, right, &newLeft, &newRight);
        *outLeft = HandleTypeExtension(left, newLeft);
        *right = HandleTypeExtension(right, newRight);
        return;
    }
}

ExprRet SemanticAnalyzer::HandleTypeExtension(ExprRet *src, BuiltIn::Type newType)
{
    if(src->type == newType)
    {
        return *src;
    }
    
    ExprRet out = {};
    out.type = newType;

    // if values have the same rank then they signed/unsigned with the same bit lengths,
    // in this case no extension code is to be generated
    if(GetIntRank(src->type) == GetIntRank(newType))
    {
        out.id = src->id;
    }
    else if(isSigned(src->type))
    {
        out.id = codeGen.EmitLocalSignExt(newType, src->type, src->id);
    }
    else
    {
        out.id = codeGen.EmitLocalZeroExt(newType, src->type, src->id);
    }

    return out;
}
