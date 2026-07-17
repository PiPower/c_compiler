#include "SemanticAnalysis.hpp"
#include <string.h>
#include <limits>
#include <fcntl.h>
#include <unistd.h>
#include "../utils/DataEncoder.hpp"
#include "../utils/Logger.hpp"
#include "../utils/Misc.hpp"
#include "ExpressionTypes.hpp"
#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);
constexpr StructDesc emptyDesc = {NOT_EMITTED, 0, nullptr, nullptr, nullptr};
typedef const Ast::Node Node;

constexpr inline bool canEmitPassByValue(bool usesRightValue, int usedIntRegs)
{
    // sysv ABI
    if(usedIntRegs >= 6)
    {
        return false;
    }

    if(usesRightValue && usedIntRegs >= 5)
    {
        return false;
    }

    return true;
}

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
    symTab->AddSymbol<SymbolType>(kTypeNames[0], BuiltIn::void_t, true, true, 0, 0, emptyDesc);

    // integer types
    symTab->AddSymbol<SymbolType>(kTypeNames[1], BuiltIn::s_char_8, true, true, 1, 1, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[2], BuiltIn::s_char_8, true, true, 1, 1, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[3], BuiltIn::u_char_8, true, true, 1, 1, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[4], BuiltIn::s_int_16, true, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[5], BuiltIn::s_int_16, true, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[6], BuiltIn::s_int_16, true, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[7], BuiltIn::s_int_16, true, true, 2, 2, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[8], BuiltIn::u_int_16, true, true, 2, 2, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[9], BuiltIn::u_int_16, true, true, 2, 2, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[10], BuiltIn::s_int_32, true, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[11], BuiltIn::s_int_32, true, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[12], BuiltIn::s_int_32, true, true, 4, 4, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[13], BuiltIn::u_int_32, true, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[14], BuiltIn::u_int_32, true, true, 4, 4, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[15], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[16], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[17], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[18], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[19], BuiltIn::u_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[20], BuiltIn::u_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[21], BuiltIn::u_int_64, true, true, 8, 8, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[22], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[23], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[24], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[25], BuiltIn::s_int_64, true, true, 8, 8, emptyDesc);

    symTab->AddSymbol<SymbolType>(kTypeNames[26], BuiltIn::u_int_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[27], BuiltIn::u_int_64, true, true, 8, 8, emptyDesc);

    // floats
    symTab->AddSymbol<SymbolType>(kTypeNames[28], BuiltIn::float_32, true, true, 4, 4, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[29], BuiltIn::double_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[30], BuiltIn::long_double, true, true, 16, 16, emptyDesc);

    // rest
    symTab->AddSymbol<SymbolType>(kTypeNames[31], BuiltIn::u_char_8, true, true, 1, 1, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[32], BuiltIn::complex_float_64, true, true, 8, 8, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[33], BuiltIn::complex_double_128, true, true, 16, 16, emptyDesc);
    symTab->AddSymbol<SymbolType>(kTypeNames[34], BuiltIn::complex_long_double, true, true, 32, 16, emptyDesc);

    // special built-in
    symTab->AddSymbol<SymbolType>(kTypeNames[35], BuiltIn::special, true, false, 0, 0, emptyDesc);

}

void SemanticAnalyzer::Analyze(const Ast::Node *root)
{
    // label is first cause it starts new block 
    if (root->type == Ast::st_label)
    {
        LabelStatement(root);
        return;
    }

    if(codeGen.IsBlockTerminated())
    {
        return;
    }

    if(root->type == Ast::declaration)
    {
        AnalyzeDeclaration(root->lChild, root->rChild);
    }
    else if(root->type == Ast::function_def)
    {
        AnalyzeFunctionDef(root->lChild, root->rChild);
    }
    else if(root->type == Ast::st_if)
    {
        IfStatement(root);
    } 
    else if(root->type == Ast::st_break)
    {
        BreakStatement(root);
    }
    else if(root->type == Ast::st_goto)
    {
        GotoStatement(root);
    }
    else if(root->type == Ast::st_return)
    {
        RetStatement(root);
    } 
    else if(root->type == Ast::st_switch)
    {
        SwitchStatement(root);
    }
    else if(root->type == Ast::st_while_loop)
    {
        WhileStatement(root);
    } 
    else if(root->type == Ast::st_for_loop)
    {
        ForLoopStatement(root);
    }
    else if(root->type == Ast::st_do_while_loop)
    {
        DoWhileStatement(root);
    }
    else if(root->type == Ast::st_compound)
    {

        symTab->CreateNewScope(Scope::LOCAL);
        const Ast::Node* st = root->rChild;
        while (st)
        {
            Analyze(st->lChild);
            if(st->type == Ast::st_break || st->type == Ast::st_continue)
            {
                break;
            }
            st = st->rChild;
        }

        symTab->PopScope();
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
    if(fnSym != nullptr && fnSym->isDefined)
    {
        IssueWarning(&fnDecl.token, "Function redefinition")
    }
    AnalyzeFunctionDecl(&declSpec, &fnDecl);
    fnSym = symTab->QueryFunctionSymbol(fnDecl.name);
    fnSym->isDefined = 1;
    StartFunction(fnSym, false);
    // emit function body
    const Ast::Node* bodyNode = body->rChild;
    while (bodyNode)
    {
        Analyze(bodyNode->lChild);
        if(!bodyNode->rChild)
        {
            if(bodyNode->lChild->type != Ast::st_return)
            {
                codeGen.EmitLocalJump(currFn.retIdx);
            }
            break;
        }
        bodyNode = bodyNode->rChild;
    }

    if(!bodyNode)
    {
        codeGen.EmitLocalJump(currFn.retIdx);
    }

    StopFunction(false);    
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

    if(IsArray(&decl->accArr) && !IsPointer(&decl->accArr) )
    {
        IssueWarning(&decl->token, "Fuction is not allowed to return an array")
    }
    BuiltIn::Type retType = IsPointer(&decl->accArr) ? BuiltIn::ptr : spec->symType->dType;

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
        symTab->AddSymbol<SymbolFunction>(decl->name, *spec, *decl, FnDecl->fnDecl.paramCount, 0, FnDecl->fnDecl.paramTypeList, nullptr, retType, 0, nullptr, 0, 1, 0);
        tableSym = symTab->QueryFunctionSymbol(decl->name);
    }
}

Declarator SemanticAnalyzer::ProcessDecl(const Ast::Node *declarator, std::stack<const Ast::Node*>* accessTypes, bool isAbstract, const AccessArray* typedefAcc)
{
    Declarator decl = {};
    decl.token = declarator->token;
    if(!isAbstract)
    {
        decl.name = GetViewForToken(accessTypes->top()->token, manager);
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

        if(accessType->type == Ast::direct_declarator || 
           accessType->type == Ast::direct_abstract_declarator)
        {
            accessType = accessType->rChild;
        }

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
            if(params.size() == 0) { IssueWarning(&glueNode->token, "Variadic function must have atleast on arguments")} 
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
        if(DecaysToPointer(&decl.accArr))
        {
            PointerDesc ptrDesc;
            ptrDesc.accessTypes = decl.accArr;
            ptrDesc.spec = *declSpec;
            symTab->AddSymbol<SymbolType>(decl.name, BuiltIn::ptr, true, true, 8, 8, ptrDesc);
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
        spec->typenameView = GetViewForToken(structTree->lChild->token, manager);
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
        symTab->AddSymbol<SymbolType>(spec->typenameView, symType, false, true, 0, 0, emptyDesc );
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
        symTab->AddSymbol<SymbolType>(spec->typenameView, BuiltIn::struct_t, 0, 1, 0, 0, emptyDesc);
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
            // if elemnt is passed by stack it propagates to parent
            if(!( DecaysToPointer(&members[idx].accArr))  && memType->passByValue == 0 )
            {
                sym->passByValue = 0;
            }
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
    sym->alignmentPadd = structSize % sym->alignment == 0 ? 0 : sym->alignment - structSize % sym->alignment; // pas size to make it aligned for arrays
    sym->size = structSize + sym->alignmentPadd;
    if(sym->size > 16)
    {
        sym->passByValue = 0;
    }
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
            if(currDecl->type == Ast::direct_declarator)
            {
                accessTypes.push(currDecl);
            }
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
        symTab->AddSymbol<SymbolType>(GetViewForToken(enumTree->token, manager), BuiltIn::enum_t, true, true, 4, 4, emptyDesc);
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
        std::string_view enumName = GetViewForToken(enumerator->token, manager);
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

std::vector<bool> SemanticAnalyzer::AnalyzeFunctionParams(const DeclSpecs *declSpec, const Declarator* fnDecl, int* usedIntReg, bool startFunctionBody)
{
    // sysv abi
    // if usedIntegerValues > 6 structs that were to be passed by value are passed by ptr 
    int usedIntegerValues = 0;
    if(declSpec->symType->passByValue == 0)
    {
        int64_t flag = 0;
        const std::string_view firstParamTypeName = fnDecl->accArr.ptr[0].fnDecl.paramTypeList[0].spec.typenameView;
        if(isVoidCall(fnDecl->accArr.ptr[0].fnDecl.paramCount, &fnDecl->accArr, firstParamTypeName))
        {
            flag += fpIsLast;
        }
        codeGen.EmitReturnByPtr(declSpec->symType, declSpec->typenameView, flag);
        usedIntegerValues++;
    }

    if(fnDecl->accArr.ptr[0].type != ACC_FN_DECL)
    {
        IssueWarning(nullptr, "First access type must be ACC_FN_DECL")
    }

    ParamTuple out = IterateOverParams(&fnDecl->accArr.ptr[0].fnDecl, &usedIntegerValues);
    std::vector<ParamDesc> paramDesc = std::move(std::get<0>(out));
    std::vector<bool> passByValue = std::move(std::get<1>(out));


    if(usedIntReg)
    {
        *usedIntReg = usedIntegerValues;
    }
    // allocate reserved index
    codeGen.GetIdxForLocalVar();
    codeGen.CloseParamList(!startFunctionBody);
    if(!startFunctionBody)
    {
        return passByValue;
    }
    codeGen.EmitFunctionBodyStart();
    const FnDecl* paramDecl = &fnDecl->accArr.ptr[0].fnDecl;
    for(size_t i =0; i < paramDecl->paramCount; i++)
    {
        const FunctionParams* param = &paramDecl->paramTypeList[i];
        if(param->spec.typenameView == "void")
        {
            break;
        }

        const ParamDesc& currentParam = paramDesc[i]; 
        if(currentParam.rType == BuiltIn::none &&  !isStructOrUnion(currentParam.lType))
        {
            AnalyzeLocalVarDecl(&param->spec, &param->decl);
            SymbolVariable* symVar = symTab->QueryVarSymbol(param->decl.name);
            codeGen.EmitLocalStorage(currentParam.lType, GetBuiltInAlignment(currentParam.lType), symVar->varIdx, currentParam.lIdx);
        }
        else if(currentParam.lType == BuiltIn::struct_t || currentParam.lType == BuiltIn::union_t)
        {
            AnalyzeLocalVarDecl(&param->spec, &param->decl, false, currentParam.lIdx); // by idx to variable 
        }
        else
        {
            AnalyzeLocalVarDecl(&param->spec, &param->decl);
            SymbolVariable* symVar = symTab->QueryVarSymbol(param->decl.name);
            codeGen.CopyPassTmpStructToStruct(symVar->varIdx, symVar->spec.symType->size, currentParam.lType, currentParam.rType, 
                                    symVar->spec.symType->alignment, currentParam.lIdx, currentParam.rIdx);           
        }

    }
    return passByValue;
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
        handyString += GetViewForToken(currChild->token, manager);
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

int SemanticAnalyzer::TryEmitValueStruct(const StructDesc& str, bool isLast, int usedValueCount, ParamDesc* paramDesc)
{
    // sysv abi: no free register to pass struct even if it consists of only one int
    ByValueStructDesc desc= codeGen.BuildValueStruct(str);
    int64_t lIdx = codeGen.GetIdxForLocalVar();
    int64_t rIdx = desc.rType == "" ? INDEX_INVALID : codeGen.GetIdxForLocalVar();
    // struct must be passed via ptr

    if(!canEmitPassByValue(rIdx != INDEX_INVALID, usedValueCount))
    {
        return 0;
    }

    BuiltIn::Type lType = GetBuiltInType(desc.lType);
    BuiltIn::Type rType = GetBuiltInType(desc.rType);

    int8_t isLastJoined = isLast && rType == BuiltIn::none ? fpIsLast : 0;
    codeGen.EmitFunctionParam(lType, isLastJoined, {lIdx, {}});
    if(rType != BuiltIn::none)
    {
        codeGen.EmitFunctionParam(rType, isLast,  {rIdx, {}});
    }

    if(paramDesc)
    {
        *paramDesc = {lType, rType, lIdx, rIdx};
    }
    return rIdx == INDEX_INVALID ? 1 : 2;
}

ExprRet SemanticAnalyzer::LoadVariable(const ExprRet &ret)
{
    ExprRet out = ret;
    if(out.id == EXPR_ID_VAR)
    {
        const SymbolVariable* symVar = out.var; 
        if(symVar->varIdx == EXPR_ID_GLOBAL)
        {
            out.id = codeGen.EmitLocalGlLoad(symVar->spec.symType->dType, symVar->spec.symType->alignment, symVar->decl.name);
            out.var = nullptr;
        }
        else
        {
            out.id = codeGen.EmitLocalLoad(symVar->spec.symType->dType, symVar->spec.symType->alignment, symVar->varIdx);
            out.var = nullptr;
        }
    }

    return out;
}

void SemanticAnalyzer::WriteCodeToFile(const char *filename)
{
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC,  
                  S_IRUSR | S_IWUSR |  S_IRGRP | S_IWGRP |  S_IROTH | S_IWOTH);  
    codeGen.WriteToFile(fd);
    close(fd);
}

void SemanticAnalyzer::HandleNotZeroComparison(const ExprRet &cond, int64_t bodyLabel, int64_t exitLabel)
{
        if(cond.id == EXPR_ID_CONST)
    {
        Typed::Number num = {};
        num.type = cond.num.type;
        if(cond.num != num)
        {
            codeGen.EmitLocalJump(bodyLabel);
        }
        else
        {
            codeGen.EmitLocalJump(exitLabel);
        }
    }
    else
    {
        int64_t id = HandleNotEqZero(cond);
        codeGen.EmitLocalCondJump(id, bodyLabel, exitLabel);
    }
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
        if(symVar->spec.declType.spec.extern_ == 0)
        {
            InitGlobalVar(symVar);
        }
    }

    for(SymbolFunction* symFn : maybeUndefinedFuncs)
    {
        if(symFn->isDefined == 0)
        {
            StartFunction(symFn, true);
            StopFunction(true);
        }
    }
}

void SemanticAnalyzer::ResolveIntegralPromotion(const BuiltIn::Type& left, const BuiltIn::Type& right, BuiltIn::Type* outLeft, BuiltIn::Type* outRight)
{
    if(isSmallInteger(left) && (isSmallInteger(right)))
    {
        *outLeft = BuiltIn::s_int_32;
        *outRight = BuiltIn::s_int_32;
        return;
    }

    if(left == right)
    {
        *outLeft = left;
        *outRight= right;
        return;
    }

    int rankLeft = GetIntRank(left);
    int rankRight = GetIntRank(right);
    int maxRank = std::max(rankLeft, rankRight);
    // signed values are even and unsigned are odd
    if(left %2 == right%2)
    {
        if(rankLeft > rankRight)
        {
            *outLeft = left;
            *outRight= left;
        }
        else
        {
            *outLeft = right;
            *outRight= right;
        }
        return;
    }
    // here we know that we have mixed types
    const BuiltIn::Type* unsignedType = isUnsigned(left) ? &left : nullptr;
    unsignedType = unsignedType == nullptr ? &right : unsignedType;
    const BuiltIn::Type* signedType = isSigned(left) ? &left : nullptr;
    signedType = signedType == nullptr? &right : signedType;

    if(GetIntRank(*unsignedType) == maxRank)
    {
        *outLeft = *unsignedType;
        *outRight = *unsignedType;
        return;
    }
    else
    {
        *outLeft = *signedType;
        *outRight = *signedType;
        return;
    }

}

int SemanticAnalyzer::GetIntRank(BuiltIn::Type type)
{
    static const int intRanks[] = {0, 1, 1, 2, 2, 3, 3, 4, 4}; // i1, i8, ui8, i16, ui16, i32, ui32, i64, ui64
    if(type < BuiltIn::int_1 || type > BuiltIn::u_int_64)
    {
        return -1;
    }

    return  intRanks[type - BuiltIn::s_char_8];
}

void SemanticAnalyzer::BinaryExprProlog(ExprRet *left, ExprRet *right, const Ast::Node *leftTerm, const Ast::Node *rightTerm)
{
    ExprRet oldLeft = LoadVariable(AnalyzeExpr(leftTerm));
    ExprRet oldRight = LoadVariable(AnalyzeExpr(rightTerm));
    HandleTypePromotion(&oldLeft, &oldRight, left, right);
}

int64_t SemanticAnalyzer::PointerArg(const FunctionParams& param, const ExprRet& result)
{
    if(DecaysToPointer(&param.decl.accArr) || param.spec.declType.isEllipsis)
    {
        if(result.type == BuiltIn::string)
        {
            return result.id;
        }
        else if(result.id == EXPR_ID_FN)
        {
            int64_t store = codeGen.GetIdxForLocalVar();
            codeGen.EmitLocalNamedStore(BuiltIn::ptr, 8, store, result.fn->decl.name);
            return codeGen.EmitLocalLoad(BuiltIn::ptr, 8, store);
        }
        else
        {
            uint64_t arrayOrder = GetArrayOrder(&result.var->decl.accArr);
            int64_t id = result.id == EXPR_ID_VAR ? result.var->varIdx : result.id;
            return result.type == BuiltIn::ptr ? 
                codeGen.EmitLocalLoad(BuiltIn::ptr, 8, id) : 
                codeGen.EmitLocalArrGetElemPtr(&result.var->decl.accArr, result.var->spec.typenameView,
                    result.var->varIdx, std::vector<uint64_t>(arrayOrder, 0) );
        }
    }
}

void SemanticAnalyzer::StructArg(
        const SymbolFunction& symFn,
        size_t argIdx, 
        const FunctionParams& param,
        const ExprRet& result, 
        const Ast::Node* callRoot,
        ArgDesc* left,
        ArgDesc* right,
        int usedIntRegs)
{
    *left = {};
    *right = {};
    // in the case of ellipsis we cannot use passByValueArray, we need to check each arg separetly
    if(param.spec.declType.isEllipsis || IsArgPassedByValue(symFn.passByValueArray, argIdx))
    {
        if(result.id != EXPR_ID_VAR)
        {
            IssueWarning(&callRoot->token, "Struct cannot be passed as expression")
        }
        ByValueStructDesc desc = codeGen.BuildValueStruct(param.spec.symType->str);
        
        // check needs to be done only in ellipsis arg
        bool canEmit = param.spec.declType.isEllipsis == 1 ? 
                        canEmitPassByValue(desc.rType != "", usedIntRegs) : true; 
        if(canEmit && desc.rType == "")
        {
            int64_t loadedVar = codeGen.EmitLocalLoad(desc.lType, param.spec.symType->alignment, result.var->varIdx);
            left->op = {loadedVar, {}};
            left->paramType = GetBuiltInType(desc.lType);
            return;
        }
        else if(canEmit)
        {
            std::string retName = codeGen.getRetName(&param.spec, nullptr);
            
            ////DODAĆ Kopiowanie i używanie struktury tmp
            //int32_t structAlignment = param.spec.symType->alignment;
            //int64_t tmp = codeGen.AllocatePassByTmpStruct(desc.lType, desc.rType, structAlignment);
            //codeGen.EmitLocalIntMemcpy(structAlignment, structAlignment, tmp, result.var->varIdx, param.spec.symType->size);
            // TODO: Verify if there needs to be a copy of source type
            std::vector<uint64_t> indicies({0});
            int64_t lPtr = codeGen.EmitLocalArrGetElemPtr(nullptr, retName,  result.var->varIdx, indicies);
            int64_t l = codeGen.EmitLocalLoad(desc.lType, param.spec.symType->alignment, lPtr);
            indicies[0] = 1;
            int64_t rPtr = codeGen.EmitLocalArrGetElemPtr(nullptr, retName,  result.var->varIdx, indicies);
            int64_t r = codeGen.EmitLocalLoad(desc.rType, param.spec.symType->alignment, rPtr);
            left->paramType = GetBuiltInType(desc.lType);
            left->op = {l};
            left->parmIdx = argIdx;

            right->paramType = GetBuiltInType(desc.rType);
            right->op = {r};
            right->parmIdx = argIdx;
            return;
        }
    }
 
    //Fill structs passed by ptr
    if(result.id != EXPR_ID_VAR)
    {
        IssueWarning(&callRoot->token, "Struct must be a variable")
    }

    int64_t tmp = codeGen.AllocateLocalVariable(BuiltIn::struct_t, result.var->spec.symType, result.var->spec.typenameView);
    uint32_t alignment = result.var->spec.symType->alignment;
    codeGen.EmitLocalIntMemcpy(alignment, alignment, tmp, result.var->varIdx, param.spec.symType->size);
    left->paramType = BuiltIn::struct_t;
    left->op.idx = tmp;
    left->parmIdx = argIdx;
}

Operator SemanticAnalyzer::ValueArg(const FunctionParams &param, const ExprRet &result)
{
    ExprRet res = LoadVariable(result);
    res = HandleTypeConversion(&result, param.spec.symType->dType);
    return {result.id, result.num};
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
        symTab->AddSymbol<SymbolVariable>(decl->name, symTab->currentTable->scopeType, spec, decl, &opts, EXPR_ID_GLOBAL);
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
        symVar->decl.initExpr = decl->initExpr; // set init expr
        codeGen.EmitGlobalVariable(spec, decl);
        if(spec->declType.spec.extern_ == 0)
        {
            InitGlobalVar(symVar);
        }

        if(uninitGlobals.find(symVar) != uninitGlobals.end())
        {
            uninitGlobals.erase(symVar);
        }
    }
}

void SemanticAnalyzer::InitGlobalVar(const SymbolVariable* symVar)
{
    const DeclSpecs* spec = &symVar->spec;
    const Declarator* decl = &symVar->decl;

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
    InitArray(&decl->accArr, decl->initExpr, symVar, nullptr);

    codeGen.EmitGLobalArrayAlignment(IsPointer(&decl->accArr), spec->symType->alignment);
}

int64_t SemanticAnalyzer::AnalyzeFnCallStart(const Ast::Node* callRoot, const SymbolFunction* symFn, const std::string_view& fnName)
{
    std::vector<BuiltIn::Type> paramList;
    if(symFn->params[symFn->paramCount - 1].spec.declType.isEllipsis)
    {
        // first int is reserved for struct if it is passed via pointer, remaining params
        // will be resolved by IterateOverParamsForBuilints
        int usedInts = 0;
        if(isStructOrUnion(symFn->retType) && symFn->spec.symType->passByValue == 0)
        {
            usedInts++;
            paramList.push_back(BuiltIn::ptr);
            std::vector<BuiltIn::Type> paramListTmp = IterateOverParamsForBuilints(&symFn->decl.accArr.ptr[0].fnDecl, &usedInts);
            paramList.insert(paramListTmp.begin(), paramListTmp.end(), paramList.begin());
        }
        else
        {
            paramList = IterateOverParamsForBuilints(&symFn->decl.accArr.ptr[0].fnDecl, &usedInts);
        }
    }

    if(!isStructOrUnion(symFn->retType))
    {
        return codeGen.EmitOpenFnCall(symFn->retType, fnName, nullptr, paramList.size(), paramList.data());
    }
    if(symFn->spec.symType->passByValue)
    {
        int id = codeGen.EmitOpenFnCall(BuiltIn::special, fnName, &symFn->spec, paramList.size(), paramList.data());
        return id;
    }
    else
    {
        int64_t tmp = codeGen.AllocateLocalVariable(symFn->retType, symFn->spec.symType, symFn->spec.typenameView);
        int id = codeGen.EmitOpenFnCall(BuiltIn::void_t, fnName, nullptr, paramList.size(), paramList.data());
        int64_t flags = fpIsUsedInCall;
        const std::string_view firstParamTypeName = symFn->decl.accArr.ptr[0].fnDecl.paramTypeList[0].spec.typenameView;
        if(isVoidCall(symFn->paramCount, &symFn->decl.accArr, firstParamTypeName))
        {
            flags += fpIsLast;
        }
        codeGen.EmitReturnByPtr(symFn->spec.symType, symFn->spec.typenameView, flags, tmp);
        return id;
    }
}

ParamTuple SemanticAnalyzer::IterateOverParams(const FnDecl* paramDecl, int* usedInts)
{
    ParamTuple out{};
    std::vector<ParamDesc>& paramDesc = std::get<0>(out);
    std::vector<bool>& passByValue = std::get<1>(out);

    passByValue.reserve(paramDecl->paramCount);
    paramDesc.reserve(paramDecl->paramCount);

    for(size_t i =0; i < paramDecl->paramCount; i++)
    {
        const FunctionParams* param = &paramDecl->paramTypeList[i];
        int8_t isLast = i == paramDecl->paramCount - 1 ? fpIsLast : 0;
        if(param->spec.typenameView == "void")
        {
            break;
        }
        else if(param->spec.declType.isEllipsis)
        {
            codeGen.EmitFunctionParam(BuiltIn::special, isLast, {});
            break;
        }
        if(DecaysToPointer(&param->decl.accArr))
        {
            int64_t idx = codeGen.GetIdxForLocalVar();
            codeGen.EmitFunctionParam(BuiltIn::ptr, isLast, {idx, {}});
            paramDesc.emplace_back(BuiltIn::ptr, BuiltIn::none, idx);
            (*usedInts)++;
            passByValue.push_back(true);
        }
        else if(param->spec.symType->dType != BuiltIn::struct_t && param->spec.symType->dType != BuiltIn::union_t)
        {
            int64_t idx = codeGen.GetIdxForLocalVar();
            codeGen.EmitFunctionParam(param->spec.symType->dType, isLast, {idx, {}});
            paramDesc.emplace_back(param->spec.symType->dType, BuiltIn::none, idx);
            passByValue.push_back(true);
            if(isInteger(param->spec.symType->dType))
            {
                (*usedInts)++;
            }
        }
        else
        {
            int usedValueSlots = 0;
            ParamDesc desc = {};
            if(param->spec.symType->passByValue == 0 || 
                (usedValueSlots = TryEmitValueStruct(param->spec.symType->str, isLast, *usedInts, &desc)) == 0 )
            {
                int64_t idx = codeGen.GetIdxForLocalVar();
                codeGen.EmitFunctionParam(param->spec.symType, param->spec.typenameView, isLast, idx);
                desc = {BuiltIn::struct_t, BuiltIn::none, idx, INDEX_INVALID};
            }
            passByValue.push_back(desc.lType != BuiltIn::struct_t);
            paramDesc.push_back(desc);
            *usedInts += usedValueSlots;
        }

    }
    return out;
}

std::vector<BuiltIn::Type> SemanticAnalyzer::IterateOverParamsForBuilints(const FnDecl *paramDecl, int *usedInts)
{
    int usedIntCount = *usedInts;
    std::vector<BuiltIn::Type> out;
    out.reserve(15);
    for(size_t i =0; i < paramDecl->paramCount; i++)
    {
        const FunctionParams* param = &paramDecl->paramTypeList[i];
        if(param->spec.typenameView == "void" || param->spec.declType.isEllipsis)
        {
            break;
        }

        if(DecaysToPointer(&param->decl.accArr))
        {
            out.push_back(BuiltIn::ptr);
            usedIntCount++;
        }
        else if(param->spec.symType->dType != BuiltIn::struct_t && param->spec.symType->dType != BuiltIn::union_t)
        {
            out.push_back(param->spec.symType->dType);
            if(isInteger(param->spec.symType->dType))
            {
                usedIntCount++;
            }
        }
        else
        {
            bool passViaPtr = true;
            ByValueStructDesc desc = codeGen.BuildValueStruct(param->spec.symType->str);
            if(!canEmitPassByValue(desc.rType != "", usedIntCount))
            {
                passViaPtr = false;
            }
            if(passViaPtr)
            {
                out.push_back(BuiltIn::ptr);
                usedIntCount++;
            }
            else
            {
                out.push_back(GetBuiltInType(desc.lType));
                usedIntCount++;
                if(desc.rType != "")
                {
                    out.push_back(GetBuiltInType(desc.rType));
                    usedIntCount++;
                }
            }
        }

    }
    return out;
}

std::vector<ArgDesc> SemanticAnalyzer::AnalyzeFnCallArgs(const Ast::Node* callRoot, const SymbolFunction* symFn)
{
    std::vector<ArgDesc> args;
    if(!callRoot->rChild)
    {
        return args;
    }
    const Ast::Node* arg = callRoot->rChild->rChild;
    const FunctionParams* params = symFn->params;
    size_t i = 0;
    int usedIntRegs = symFn->usedIntRegs;
    while (arg)
    {
        if(i >= symFn->paramCount)
        {
            IssueWarning(&callRoot->token, "Incorrect number of function call arguments")
        }
        ExprRet result = AnalyzeExpr(arg->lChild);
        ArgDesc argDesc = {};
        argDesc.parmIdx = i;
        int isEllipsis = params[i].spec.declType.isEllipsis;

        if(result.type == BuiltIn::ptr ||
           result.type == BuiltIn::string ||
           result.type == BuiltIn::array)
        {
            argDesc.paramType = result.type == BuiltIn::string ? BuiltIn::string : BuiltIn::ptr;
            argDesc.op.idx = PointerArg(params[i], result);
            usedIntRegs += isEllipsis;
        }
        else if(isStructOrUnion(result.type))
        {
            ArgDesc left= {}, right = {};
            StructArg(*symFn, i, params[i], result, callRoot, &left, &right, usedIntRegs);
            argDesc.op = left.op;
            argDesc.paramType = left.paramType;
            
            usedIntRegs += isEllipsis * isInteger(argDesc.paramType);
            if(right.paramType != BuiltIn::none)
            {
                args.push_back(argDesc);
                argDesc.op = right.op;
                argDesc.paramType = right.paramType;
                usedIntRegs += isEllipsis * isInteger(argDesc.paramType);
            }
        }
        else
        {
            if(params[i].spec.declType.isEllipsis)
            {
                ExprRet res = LoadVariable(result);
                argDesc.op = {res.id, res.num};
                argDesc.paramType = res.type;
                usedIntRegs += isInteger(argDesc.paramType);
            }
            else
            {
                argDesc.paramType = params[i].spec.symType->dType; 
                argDesc.op = ValueArg(params[i], result);
            }
        }

        args.push_back(argDesc);
        arg = arg->rChild;
        if(params[i].spec.declType.isEllipsis == 0)
        {
            i++;
        }
    }

    bool isEllipsis = symFn->params[symFn->paramCount - 1].spec.declType.isEllipsis;
    size_t argCount = isEllipsis ? symFn->paramCount - 1 : symFn->paramCount;
    if(i != argCount )
    {
        IssueWarning(&callRoot->token, "Incorrect number of function call arguments")
    }

    return args;
}

void SemanticAnalyzer::InitArray(
    const AccessArray *accArr,
    const Ast::Node *initExpr,
    const SymbolVariable* symVar, 
    std::vector<uint64_t>* parentPosition)
{
    const DeclSpecs* spec = &symVar->spec;
    const Declarator* decl = &symVar->decl;

    if(spec->symType->dType == BuiltIn::struct_t || spec->symType->dType == BuiltIn::union_t )
    {
        IssueWarning(nullptr, "Array of non built-in types are not supported")
    }

    // init arrays
    const AccessArray nextAcc = {accArr->ptr + 1, accArr->count - 1};
    bool isNestedArray = nextAcc.count > 0 && IsArray(&nextAcc);
    bool isGlobal = parentPosition == nullptr;
    // here we emit type
    if(accArr->count == 0 || (accArr->ptr[0].type == ACC_POINTER))
    {
        // for globals ExprRet is useless
        AnalyzeInitializer(isGlobal, spec, &nextAcc, initExpr->lChild->lChild, false);
        return;
    }

    std::vector<ArrayInitPair> pairs;
    if(initExpr)
    {   
        pairs = PartitionArrayInitializer(initExpr, &nextAcc, &logger, &ne, &utilHeap);
    }

    uint64_t currentPair = 0;
    uint64_t arraySize = accArr->ptr->array.size;
    
    if(isGlobal)
    {
        codeGen.StartArray();
        for(uint64_t i = 0; i < arraySize; i++)
        {
            codeGen.EmitDeclaratorAcc(&nextAcc, &spec->typenameView);
            if(currentPair < pairs.size() && i == pairs[currentPair].idx)
            {
                InitArray(&nextAcc, pairs[currentPair].initializerList, symVar, nullptr);
                currentPair++;
            }
            else
            {
                // for globals ExprRet is useless
                AnalyzeInitializer(isGlobal, spec, &nextAcc, nullptr, isNestedArray);
            }

            if(i < arraySize - 1)
            {
                codeGen.ArgSeparator();
            }
        }
        codeGen.EndArray();
        return;
    }

    if(pairs.size() == 0)
    {
        return;
    }

    for(uint64_t i = 0; i < arraySize; i++)
    {
        if(currentPair < pairs.size() && i == pairs[currentPair].idx)
        {
            if(nextAcc.count == 0 || (nextAcc.ptr[0].type == ACC_POINTER))
            {   

                ExprRet src = AnalyzeInitializer(isGlobal, spec, &nextAcc, initExpr->lChild->lChild, false);

                ExprRet target = {};
                target.type = IsPointer(&symVar->decl.accArr) ? BuiltIn::ptr : symVar->spec.symType->dType;
                parentPosition->push_back(i);
                target.id = codeGen.EmitLocalArrGetElemPtr(&decl->accArr, spec->typenameView, symVar->varIdx, *parentPosition);
                parentPosition->pop_back();
                ResolveAssignment(target, src);
                //HandleAssignment
                //int x = 2;
            }
            else
            {
                parentPosition->push_back(i);
                InitArray(&nextAcc, pairs[currentPair].initializerList, symVar, parentPosition);
                parentPosition->pop_back();
            }
            currentPair++;
        }
    }
    
}

void SemanticAnalyzer::AnalyzeLocalVarDecl(
    const DeclSpecs *spec, 
    const Declarator *decl,
    bool needsEmission,
    int64_t preallocatedIdx)
{
    int64_t ownerId = 0;
    SymbolVariable* symVar = symTab->QueryVarSymbol(decl->name, &ownerId);

    if(symVar == nullptr || ownerId != symTab->currentTable->id)
    {
        VariableOpts opts = {.isEnumerator = 0, .isConst = 0, .isEmitted = 0};
        if(preallocatedIdx == INDEX_INVALID)
        {
            preallocatedIdx = codeGen.GetIdxForLocalVar();
        }
        symTab->AddSymbol<SymbolVariable>(decl->name, symTab->currentTable->scopeType, spec, decl, &opts, preallocatedIdx);
        symVar = symTab->QueryVarSymbol(decl->name);
    }
    else
    {
        IssueWarning(&decl->token, "Local variable redefinition")
    }

    // if needsEmission == false it is assumed that variable is already emitted
    symVar->opts.isEmitted = 1;
    if(needsEmission)
    {
        codeGen.EmitLocalVariable(symVar);
        InitLocalVariable(symVar);
    }
}

void SemanticAnalyzer::StartFunction(SymbolFunction* symFn, bool declareFunc)
{
    // emitt type 
    if(!IsPointer(&symFn->decl.accArr , 1))
    {
        codeGen.EmitUnionStruct(symFn->spec.symType, symFn->spec.typenameView);
    }

    codeGen.EmitFunctionName(&symFn->spec, &symFn->decl, declareFunc);
    symTab->CreateNewScope(Scope::LOCAL);
    symFn->fnScope = symTab->currentTable;
  
    // emit parameters  
    std::vector<bool> passByValueArray = AnalyzeFunctionParams(&symFn->spec, &symFn->decl, &symFn->usedIntRegs, !declareFunc);
    symFn->passByValueArray = symTab->AllocateTypeArrayOnHeap<uint8_t>(passByValueArray.size()/8 + 1);
    SetByValueArray(symFn->passByValueArray, passByValueArray);

    // emitt type 
    currFn.symFn = symFn;
    currFn.breakCalled = false;
    currFn.retIdx = codeGen.GetIdxForLocalVar();
    const SymbolType* retType = currFn.symFn->spec.symType;
    if(!declareFunc &&  
       currFn.symFn->retType != BuiltIn::void_t && 
       (!isStructOrUnion(retType->dType) || retType->passByValue))
    {
        currFn.retVal = codeGen.AllocateLocalVariable(currFn.symFn->retType, 
                    currFn.symFn->spec.symType, currFn.symFn->spec.typenameView);
    }
    else
    {
        currFn.retVal = 0;
    }

    return;
}


void SemanticAnalyzer::StopFunction(bool declareFunc)
{
    if(!declareFunc)
    {
        codeGen.EmitFunctionBodyClose(currFn.symFn->retType, currFn.retIdx, currFn.retVal, &currFn.symFn->spec);
    }
    symTab->PopScope();
    currFn.symFn = nullptr;
    currFn.breakCalled = false;
    currFn.namedLabels.clear();
    while (currFn.labels.size() > 0)
    {
        currFn.labels.pop();
    }
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
            if( (symTab->QuerySymKinds(GetViewForToken(currNode->token, manager)) & Sym::TYPEDEF)  > 0)
            {
                SymbolTypedef* symTypedef = symTab->QueryTypedefSymbol(GetViewForToken(currNode->token, manager));
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
                spec.typenameView = GetViewForToken(currNode->token, manager);
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
    if(!IsArray(&symVar->decl.accArr))
    {
        Ast::Node node = {
            .type = Ast::init_expr,
            .lChild = const_cast<Ast::Node*>(symVar->decl.initExpr)
            };

        ExprRet initInfo;
        initInfo.type = IsPointer(&symVar->decl.accArr) ? BuiltIn::ptr : symVar->spec.symType->dType; 
        initInfo.id = EXPR_ID_VAR;
        initInfo.var = symVar;
        node.rChild = (Ast::Node*)&initInfo;

        AnalyzeExpr(&node);
    }
    else
    {
        std::vector<uint64_t> nestedIndicies;
        InitArray(&symVar->decl.accArr, symVar->decl.initExpr, symVar, &nestedIndicies);
        //codeGen.InitLocalArray(symVar->decl.name, &symVar->decl.accArr, symVar->decl.initExpr, &symVar->spec);
    }
}

ExprRet SemanticAnalyzer::ResolveAssignment(ExprRet dst, ExprRet src)
{
    if(src.id == EXPR_ID_IGNORE)
    {
        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
    }

    if(dst.type == BuiltIn::struct_t || dst.type == BuiltIn::union_t)
    {
        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
    }

    if(dst.id == EXPR_ID_VAR && dst.type != BuiltIn::ptr)
    {
        dst.id = dst.var->varIdx;
        dst.var = nullptr;
    }

    if(src.id == EXPR_ID_VAR && src.type != BuiltIn::ptr)
    {
        const SymbolVariable* srcVar = src.var;
        src.id = codeGen.EmitLocalLoad(srcVar->spec.symType->dType, srcVar->spec.symType->alignment, srcVar->varIdx);
        src.var = nullptr;
    }


    if(dst.type != BuiltIn::ptr)
    {
        return HandleSimpleAssignment(&dst, &src);
    }
    else
    {
        return HandlePointerAssignment(&dst, &src);
    }
    return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
}

ExprRet SemanticAnalyzer::AnalyzeInitializer(bool isGlobal, const DeclSpecs *spec, const AccessArray *accArr, const Ast::Node *initializer, bool isComplexType)
{
    if(!(spec->symType->dType >= BuiltIn::s_char_8 && spec->symType->dType <= BuiltIn::double_64))
    {
        //IssueWarning(nullptr, "Unsupported initializer type by codegen");
        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
    }
    if(isGlobal && !initializer)
    {
        if(isComplexType)
        {
            codeGen.EmitZeroInitType(isGlobal);
        }
        else if(isFloat(spec->symType->dType))
        {
            codeGen.EmitZeroInitFloat(isGlobal);
        }
        else
        {
            codeGen.EmitZeroInitInt(isGlobal);
        }
        return {};
    }
    else if(!initializer)
    {
        return {};
    }
    if(isGlobal)
    {

        std::string initStr;
        if(initializer->type == Ast::string_literal)
        {
            int64_t strIdx = codeGen.EmitString(initializer);
            codeGen.EmitString(true, strIdx);
        }
        else
        {
            ExprRet ret = AnalyzeExpr(initializer);
            if(ret.id != EXPR_ID_CONST)
            {
                IssueWarning(&initializer->token, "global value may only be initialized by constant expression")
            }
            codeGen.EmitConstant(true, spec->symType->dType, ret.num);
        }

        return {};
    }
    ExprRet out = {};
    if(initializer->type == Ast::string_literal)
    {
        int64_t strIdx = codeGen.EmitString(initializer);
        out.type = BuiltIn::string;
        out.id = strIdx;
    }
    else
    {
        out = AnalyzeExpr(initializer);
    }
    return out;
}

ExprRet SemanticAnalyzer::AnalyzeExpr(const Ast::Node *root)
{
    if(codeGen.IsBlockTerminated())
    {
        return {BuiltIn::none, {}, EXPR_ID_IGNORE};
    }

    if(!root)
    {
        return {BuiltIn::none, {},EXPR_ID_EMPTY};
    }
    switch (root->type)
    {
    case Ast::struct_access: return HandleStructAccess(root);
    case Ast::op_log_negate: return HandleNegate(root); 
    case Ast::function_call: return HandleFunctionCall(root);
    case Ast::get_addr: return HandleGetAddr(root);
    case Ast::assignment: return HandleAssignment(root);
    case Ast::identifier: return HandleIdentifier(root);
    case Ast::cast: return HandleCast(root);
    case Ast::op_less: return BinaryOp<CmpLess>(this, &codeGen, root);
    case Ast::op_less_equal: return BinaryOp<CmpLessEq>(this, &codeGen, root);
    case Ast::op_equal: return BinaryOp<CmpEqual>(this, &codeGen, root);
    case Ast::op_not_equal: return BinaryOp<CmpNotEqual>(this, &codeGen, root);
    case Ast::op_greater_equal: return BinaryOp<CmpGreaterEq>(this, &codeGen, root);
    case Ast::op_greater: return BinaryOp<CmpGreater>(this, &codeGen, root);
    case Ast::op_add: return BinaryOp<BinaryAddition>(this, &codeGen, root);
    case Ast::op_subtract: return BinaryOp<BinarySubtraction>(this, &codeGen, root);
    case Ast::op_multiply: return BinaryOp<BinaryMultiplication>(this, &codeGen, root);
    case Ast::op_divide: return BinaryOp<BinaryDivision>(this, &codeGen, root);
    case Ast::op_divide_modulo: return BinaryOp<BinaryModulus>(this, &codeGen, root);
    case Ast::op_and: return BinaryOp<BinaryBitAnd>(this, &codeGen, root);
    case Ast::op_inc_or: return BinaryOp<BinaryBitOr>(this, &codeGen, root);
    case Ast::op_exc_or: return BinaryOp<BinaryBitXor>(this, &codeGen, root);
    case Ast::op_l_shift: return BinaryOp<BinaryShiftLeft>(this, &codeGen, root);
    case Ast::op_r_shift: return BinaryOp<BinaryShiftRight>(this, &codeGen, root);
    case Ast::op_minus: return HandleOpMinus(root);
    case Ast::init_expr: return HandleInitExpr(root);        
    case Ast::character: return LoadCharacter(root);
    case Ast::string_literal: return LoadStringLiteral(root);
    case Ast::constant: return LoadConstant(root);            
    case Ast::compound_literal: return CompoundLiteral(root); 
    case Ast::expression: 
    {
        // if there is only one expr the result might be used, otherwise it cannot
        const Ast::Node* expr = root->rChild;
        if(!expr->rChild)
        {
            return AnalyzeExpr(expr->lChild);
        }
        while (expr)
        {
            AnalyzeExpr(expr->lChild);
            expr = expr->rChild;
        }
        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};  break;
    }break;
    default: return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};  break;
    }

    return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
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

ExprRet SemanticAnalyzer::LoadCharacter(const Ast::Node *character)
{
    std::string_view str = GetViewForToken(character->token, manager);
    Typed::Number num;
    num.type = Typed::d_int8_t;
    num.int8 = str[1];
    return ExprRet{BuiltIn::s_char_8, num, EXPR_ID_CONST};
}

ExprRet SemanticAnalyzer::LoadConstant(const Ast::Node *constant)
{
    // system here is bit simplified as only 32 and 64 bit values in
    //  linux style convention are considered
    Typed::Number num;
    BuiltIn::Type type;
    std::string_view str = GetViewForToken(constant->token, manager);
    if(constant->token.isFloat)
    {
        long double value = stringToLongDouble(str.data(), str.length(), MODE_DEC);
        if(constant->token.f)
        {
            type = BuiltIn::float_32;
            num.type = Typed::d_float;
            num.float32 = (float)value;
        }
        else if(constant->token.l)
        {
            type = BuiltIn::long_double;
            num.type = Typed::d_l_double;
            num.lFloat = (long double)value;
        }
        else
        {
            type = BuiltIn::double_64;
            num.type = Typed::d_double;
            num.float64 = (double)value;
        }
    }
    else if(constant->token.u)
    {
        uint64_t value = stringToUint64(str.data(), str.length(), MODE_DEC); 
        if(constant->token.l == 0 && value <= UINT32_MAX)
        {
            type = BuiltIn::u_int_32;
            num.type = Typed::d_uint32_t;
            num.uint32 = (uint32_t)value;
        }
        else
        {
            type = BuiltIn::u_int_64;
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
            type = BuiltIn::s_int_32;
            num.type = Typed::d_int32_t;
            num.int32 = static_cast<int32_t>(value);
        }
        else
        {
            type = BuiltIn::s_int_64;
            num.type = Typed::d_int64_t;
            num.int64 = value;
        }
    }

    return ExprRet{type, num, EXPR_ID_CONST};
}

ExprRet SemanticAnalyzer::LoadStringLiteral(const Ast::Node *string)
{
    ExprRet out;
    out.type = BuiltIn::string;
    out.id = codeGen.EmitString(string);

    return out;
}

ExprRet SemanticAnalyzer::HandleInitExpr(const Ast::Node *root)
{
    ExprRet* destHandle = (ExprRet*)root->rChild;
    ExprRet source = AnalyzeExpr(root->lChild);

    return ResolveAssignment(*destHandle, source);
}

ExprRet SemanticAnalyzer::HandleFunctionCall(const Ast::Node *root)
{
    std::string_view fnName = GetViewForToken(root->lChild->token, manager);
    SymbolFunction* symFn = symTab->QueryFunctionSymbol(fnName);
    if(!symFn)
    {
        int len = fnName.length();
        IssueWarning(&root->token, "Function '%.*s' is not declared", len, fnName.data());
    }
    if(symFn->isDefined == 0)
    {
        maybeUndefinedFuncs.insert(symFn);
    }
    std::vector<ArgDesc> args = AnalyzeFnCallArgs(root, symFn);

    int64_t id = AnalyzeFnCallStart(root, symFn, fnName);
    for(size_t i =0; i < args.size(); i++)
    {
        const ArgDesc& arg = args[i];
        int8_t flags = fpIsUsedInCall;
        flags += i == args.size() - 1 ? fpIsLast : 0; 
        if(!isStructOrUnion(arg.paramType))
        {
            codeGen.EmitFunctionParam(arg.paramType, flags, arg.op);
        }
        else
        {
            FunctionParams* param = &symFn->params[arg.parmIdx];
            codeGen.EmitFunctionParam(param->spec.symType, param->spec.typenameView, flags, arg.op.idx);
        }
    }
    codeGen.EmitCloseFnCall();

    ExprRet out = {};
    out.id = id;
    out.type = symFn->retType;
    return out;
}

ExprRet SemanticAnalyzer::HandleNegate(const Ast::Node *root)
{
    ExprRet ret = AnalyzeExpr(root->lChild);
    Typed::Number num;
    num.int8 = 1;
    num.type = Typed::d_int8_t;
    num = CastTypedNumber(ret.type, num);

    ExprRet out = {};
    out.type = ret.type;
    out.id = codeGen.EmitLocalBitXor(ret.type, {ret.id, {}}, {EXPR_ID_CONST, num});
    return out;
}

ExprRet SemanticAnalyzer::HandleCast(const Ast::Node *root)
{
    ExprRet value = AnalyzeExpr(root->lChild);
    
    const Ast::Node* castNode = root->rChild;
    while (castNode)
    {
        DeclSpecs spec = AnalyzeDeclSpec(castNode->lChild->rChild);
        Declarator decl = AnalyzeDeclarator(castNode->lChild->lChild, spec.accArr, nullptr);
        if(IsPointer(&decl.accArr) || IsArray(&decl.accArr) )
        {
            if(value.id == EXPR_ID_CONST)
            {
                value.type = BuiltIn::ptr;
                value.num.uint64 = Typed::CastTo<uint64_t>(value.num);
                value.num.type = Typed::d_uint64_t;
            }
        }
        else
        {
            if(spec.typenameView == "void")
            {
                return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
            }
        }
        castNode = castNode->rChild;
    }
    
    return value;
}

ExprRet SemanticAnalyzer::HandleStructAccess(const Ast::Node *root)
{
    std::string_view variable = GetViewForToken(root->lChild->token, manager);
    const Ast::Node* elemAst = root->rChild->type == Ast::struct_access ? 
                               root->rChild->rChild : root->rChild ; 
    std::string_view element = GetViewForToken(elemAst->token, manager);
    SymbolVariable* symVar = symTab->QueryVarSymbol(variable);
    if(!isStructOrUnion(symVar->spec.symType->dType) || DecaysToPointer(&symVar->decl.accArr))
    {
        IssueWarning(&root->token, "Element cannot be accessed")
    }

    const StructDesc& structDesc = symVar->spec.symType->str;
    ExprRet out = {}; 
    for(size_t i =0; i < structDesc.argCount; i++)
    {
        if(structDesc.memberNames[i] == element)
        {
            std::vector<uint64_t> indicies({0, i});
            out.id = codeGen.EmitLocalArrGetElemPtr(&structDesc.memberList[i].accArr, symVar->spec.typenameView, symVar->varIdx, indicies);
            out.type = structDesc.memberList[i].memberType;
        }
    }

    return out;
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
    ExprRet dst = AnalyzeExpr(root->lChild);

    return ResolveAssignment(dst, exprRes);
}

ExprRet SemanticAnalyzer::HandleSimpleAssignment(const ExprRet *dst, const ExprRet *src)
{
    if(src->id == EXPR_ID_CONST)
    {
        codeGen.EmitLocalConstAsm(dst->type, GetBuiltInAlignment(dst->type), dst->id, src->num);
        return *src;
    }

    if(isInteger(dst->type) && isInteger(src->type))
    {
        int dstRank = GetIntRank(dst->type);
        int srcRank = GetIntRank(src->type);

        ExprRet localDst = *dst, localSrc = *src;
        if(dstRank > srcRank)
        {
            // here we can use HandleTypePromotion becasue src needs to be upgraded
            HandleTypePromotion(dst, src, &localDst, &localSrc);
        }
        else if(dstRank < srcRank )
        {   
            localSrc.id = codeGen.EmitLocalIntTruncate(dst->type, src->type, {src->id, src->num});
        }

        codeGen.EmitLocalStorage(localDst.type, GetBuiltInAlignment(localDst.type), localDst.id, localSrc.id);
    }
    else if(isFloat(dst->type) && isFloat(src->type))
    {
        ExprRet localDst = *dst, localSrc = *src;

        // TODO add float conversion code
        codeGen.EmitLocalStorage(localDst.type, GetBuiltInAlignment(localDst.type), localDst.id, localSrc.id);
    }
    return *src;
}

ExprRet SemanticAnalyzer::HandlePointerAssignment(const ExprRet *dst, const ExprRet *src)
{
    int64_t dstId = dst->id;
    if(dst->id == EXPR_ID_GLOBAL || dst->id == EXPR_ID_VAR)
    {
        dstId = dst->var->varIdx;
    }

    if( src->id == EXPR_ID_VAR || src->id == EXPR_ID_GLOBAL || src->id == EXPR_ID_FN)
    {
        const Declarator* decl = src->id == EXPR_ID_FN ? &src->fn->decl :  &src->var->decl;
        codeGen.EmitLocalNamedStore(src->type, GetBuiltInAlignment(src->type), dstId, decl->name);
    }
    else if(src->id == EXPR_ID_CONST && src->num.uint64 == 0)
    {
        codeGen.EmitLocalNullStorage(dstId);
    }
    else if(src->type == BuiltIn::string)
    {
        codeGen.EmitLocalStrStorage(dstId, src->id);
    }
    else
    {
       codeGen.EmitLocalStorage(dst->type, GetBuiltInAlignment(dst->type), dstId, src->id); 
    }
    return *src;
}

ExprRet SemanticAnalyzer::HandleGetAddr(const Ast::Node *root)
{
    ExprRet handle = AnalyzeExpr(root->lChild);
    if(handle.id == EXPR_ID_IGNORE)
    {
        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
    }

    if(handle.id == EXPR_ID_VAR)
    {
        return {BuiltIn::ptr, {}, handle.var->varIdx};
    }


    if(handle.type == BuiltIn::ptr)
    {

        return ExprRet{BuiltIn::none, {}, EXPR_ID_IGNORE};
    }
    return {BuiltIn::ptr, {}, EXPR_ID_IGNORE};
}

ExprRet SemanticAnalyzer::HandleIdentifier(const Ast::Node *root)
{
    std::string_view varName = GetViewForToken(root->token, manager);
    const SymbolVariable* symVar = symTab->QueryVarSymbol(varName);
    ExprRet out = {};
    if(symVar)
    {
        if(symVar->opts.isEnumerator == 0)
        {
            if(IsArray(&symVar->decl.accArr) )
            {
                out.type = BuiltIn::array;
            }
            else if(IsPointer(&symVar->decl.accArr) )
            {
                out.type = BuiltIn::ptr;
            }
            else
            {
                out.type = symVar->spec.symType->dType;
            }
            out.id = EXPR_ID_VAR;
            out.var = symVar;
        }
        else
        {
            out.id = EXPR_ID_CONST;
            out.type = BuiltIn::s_int_32;
            out.num.type = Typed::d_int32_t;
            out.num.int32 = symVar->varIdx;
        }
        return out;
    }
    else
    {
        const SymbolFunction* symFn = symTab->QueryFunctionSymbol(varName);
        out.id = EXPR_ID_FN;
        out.fn = symFn;
        out.type = BuiltIn::ptr;

        return out;
    }
}

int64_t SemanticAnalyzer::HandleNotEqZero(const ExprRet &res)
{
    Typed::Number num = {};
    num.type = BuiltInToNum(res.type);
    return codeGen.EmitLocalCmpNotEq(res.type, {res.id, res.num}, {EXPR_ID_CONST, num});
}

void SemanticAnalyzer::HandleTypePromotion(const ExprRet *left, const ExprRet *right, ExprRet *outLeft, ExprRet *outRight)
{
    if(isInteger(left->type) && isInteger(right->type))
    {
        BuiltIn::Type newLeft, newRight;
        ResolveIntegralPromotion(left->type, right->type, &newLeft, &newRight);
        *outLeft = HandleTypeConversion(left, newLeft);
        *outRight = HandleTypeConversion(right, newRight);
        return;
    }
    else if(isFloat(left->type) && isFloat(right->type))
    {
        if(left->type == right->type)
        {
            *outLeft = HandleTypeConversion(left, left->type);
            *outRight = HandleTypeConversion(right, left->type);
        }
    }
}

ExprRet SemanticAnalyzer::HandleTypeConversion(const ExprRet *src, BuiltIn::Type newType)
{
    if(src->type == newType)
    {
        return *src;
    }
    
    ExprRet out = {};
    out.type = newType;

    // if values have the same rank then they signed/unsigned with the same bit lengths,
    // in this case no extension code is to be generated
    if(src->id != EXPR_ID_CONST && isFloat(newType))
    {
        if(src->type == newType)
        {
            out.id = src->id;
        }
    }
    else if(src->id != EXPR_ID_CONST && isInteger(src->type) && isInteger(newType))
    {
        int rankSrc = GetIntRank(src->type), rankNewType = GetIntRank(newType);
        if(rankSrc == rankNewType)
        {
            out.id = src->id;
        }
        else if(rankNewType < rankSrc)
        {
            out.id = codeGen.EmitLocalIntTruncate(newType, src->type, {src->id, {}});
        }
        else if(isSigned(src->type))
        {
            out.id = codeGen.EmitLocalSignExt(newType, src->type, src->id);
        }
        else
        {
            out.id = codeGen.EmitLocalZeroExt(newType, src->type, src->id);
        }
    }
    else
    {
        out.id = EXPR_ID_CONST;
        out.num = CastTypedNumber(newType, src->num);
    }

    return out;
}

void SemanticAnalyzer::SwitchStatement(const Ast::Node *root)
{
    int64_t exitLabel = codeGen.GetIdxForLocalVar();
    currFn.labels.push(exitLabel);
    std::vector<int64_t> caseLabels;
    std::vector<Typed::Number> labelValues;
    const Ast::Node* caseNodeGlue = root->rChild->rChild;
    int64_t defaultIdx = INDEX_INVALID;
    //POPRAWIC CASE
    while (caseNodeGlue)
    {
        const Ast::Node* caseNode = caseNodeGlue->lChild;
        bool theSameLabel = false;
        // this goto is used for nested cases
RUN_CASE_CHECK:
        if(caseNode->type == Ast::st_case)
        {
            if(!theSameLabel)
            {
                caseLabels.push_back(codeGen.GetIdxForLocalVar());
            }
            else
            {
                caseLabels.push_back(caseLabels.back());
            }
            ExprRet labelVal = AnalyzeExpr(caseNode->lChild);
            if(labelVal.id != EXPR_ID_CONST)
            {
                IssueWarning(&caseNode->token, "Label for case must be constant expr")
            }
            if(!isInteger(labelVal.type))
            {
                IssueWarning(&caseNode->token, "Label for case must integer")
            }
            labelValues.push_back(labelVal.num);
        }
        else if(caseNode->type == Ast::st_default)
        {
            if(defaultIdx != INDEX_INVALID)
            {
                IssueWarning(&caseNode->token, "switch can only have one default")
            }
            defaultIdx = codeGen.GetIdxForLocalVar();
        }

        if(caseNode->rChild && caseNode->rChild->type == Ast::st_case)
        {
            caseNode = caseNode->rChild;
            theSameLabel = true;
            goto RUN_CASE_CHECK;
        }
        caseNodeGlue = caseNodeGlue->rChild;

    }
    ExprRet condExpr = LoadVariable(AnalyzeExpr(root->lChild));
    codeGen.EmitLocalSwitch(condExpr.type, condExpr.id, 
        defaultIdx == INDEX_INVALID ? exitLabel : defaultIdx, caseLabels, labelValues);
    // second pass - code gen
    caseNodeGlue = root->rChild->rChild;
    //skip everythin between switch and case
    while (caseNodeGlue && 
           caseNodeGlue->lChild && 
           (caseNodeGlue->lChild->type != Ast::st_case && caseNodeGlue->lChild->type != Ast::st_default) )
    {
        caseNodeGlue = caseNodeGlue->rChild;
    }
        
    size_t idx = 0;
    currFn.breakCalled = true;
    while (caseNodeGlue)
    {
        const Ast::Node* caseNode = caseNodeGlue->lChild;
        // reach bottom of the nested cases
        while (caseNode->rChild && caseNode->rChild->type == Ast::st_case)
        {
            caseNode = caseNode->rChild;
            // skip repeated label
            idx++;
        }
        if(caseNode->type == Ast::st_default)
        {
            if(!currFn.breakCalled)
            {
                codeGen.EmitLocalJump(defaultIdx);
            }
            currFn.breakCalled = false;
            codeGen.EmitLocalLabel(defaultIdx);
            Analyze(caseNode->lChild);
        }
        else if(caseNode->type == Ast::st_case)
        {
            if(!currFn.breakCalled)
            {
                codeGen.EmitLocalJump(caseLabels[idx]);
            }
            currFn.breakCalled = false;
            codeGen.EmitLocalLabel(caseLabels[idx]);
            idx++;
            Analyze(caseNode->rChild);
        }
        else
        {
            Analyze(caseNode);
        }
        
        caseNodeGlue = caseNodeGlue->rChild;

    }

    if(!currFn.breakCalled)
    {
        currFn.breakCalled = false;
        codeGen.EmitLocalJump(exitLabel);
    }
    codeGen.EmitLocalLabel(exitLabel);
    currFn.labels.pop();
}

void SemanticAnalyzer::IfBlock(const Ast::Node *root, int64_t exitLabel)
{
    const Ast::Node* cond = &root->rChild[0];
    const Ast::Node* statement = &root->rChild[1];
    const Ast::Node* elseClause = &root->rChild[2];

    int64_t exprLabel = codeGen.GetIdxForLocalVar();
    int64_t nextIfLabel =  elseClause->type == Ast::none ? exitLabel : codeGen.GetIdxForLocalVar();

    ExprRet condExpr = AnalyzeExpr(cond);
    int64_t condId = HandleNotEqZero(condExpr);

    codeGen.EmitLocalCondJump(condId, exprLabel, nextIfLabel);
    codeGen.EmitLocalLabel(exprLabel);
    Analyze(&root->rChild[1]);
    if(!codeGen.IsBlockTerminated())
    {
        codeGen.EmitLocalJump(exitLabel);
    }


    codeGen.EmitLocalLabel(nextIfLabel);
    if(elseClause->type == Ast::st_if)
    {
        IfBlock(elseClause, exitLabel);
    }
    else if(elseClause->type != Ast::none)
    {
        Analyze(elseClause);
        codeGen.EmitLocalJump(exitLabel);
    }
    
}

void SemanticAnalyzer::IfStatement(const Ast::Node *root)
{
    const Ast::Node* elseClause = &root->rChild[2];

    int64_t exitLabel = codeGen.GetIdxForLocalVar();

    IfBlock(root, exitLabel);

    if(elseClause->type != Ast::none)
    {
        codeGen.EmitLocalLabel(exitLabel);
    }
}

void SemanticAnalyzer::RetStatement(const Ast::Node *root)
{
    ExprRet retExpr = AnalyzeExpr(root->lChild);
    
    if(!isStructOrUnion(retExpr.type))
    {
        if(currFn.symFn->retType != BuiltIn::void_t)
        {
            ExprRet ret = HandleTypeConversion(&retExpr, currFn.symFn->retType);
            codeGen.EmitLocalBuiltInStorage(currFn.symFn->retType, 
                    GetBuiltInAlignment(currFn.symFn->retType), currFn.retVal, {retExpr.id, retExpr.num});
        }
        codeGen.EmitLocalJump(currFn.retIdx);
        return;
    }

    const SymbolFunction* symFn = currFn.symFn;
    const SymbolType* retType = symFn->spec.symType;
    int64_t id = retExpr.id;
    if(id == EXPR_ID_VAR)
    {
        id = retExpr.var->varIdx;
    }
    codeGen.EmitLocalIntMemcpy(retType->alignment, retType->alignment, currFn.retVal, id, retType->size);
    codeGen.EmitLocalJump(currFn.retIdx);
    
}
void SemanticAnalyzer::WhileStatement(const Ast::Node *root)
{

    int64_t entryLabel = codeGen.GetIdxForLocalVar();
    codeGen.EmitLocalJump(entryLabel);
    codeGen.EmitLocalLabel(entryLabel);

    int64_t bodyLabel = codeGen.GetIdxForLocalVar();
    int64_t exitLabel = codeGen.GetIdxForLocalVar();
    currFn.labels.push(exitLabel);

    ExprRet cond = AnalyzeExpr(root->lChild);
    HandleNotZeroComparison(cond, bodyLabel, exitLabel);

    codeGen.EmitLocalLabel(bodyLabel);
    Analyze(root->rChild);
    codeGen.EmitLocalJump(entryLabel);
    codeGen.EmitLocalLabel(exitLabel);

    currFn.labels.pop();
}

void SemanticAnalyzer::DoWhileStatement(const Ast::Node *root)
{
    int64_t exitLabel = codeGen.GetIdxForLocalVar();
    currFn.labels.push(exitLabel);

    int64_t entryLabel = codeGen.GetIdxForLocalVar();
    codeGen.EmitLocalJump(entryLabel);
    codeGen.EmitLocalLabel(entryLabel);
    Analyze(root->lChild);

    ExprRet cond = AnalyzeExpr(root->rChild);
    HandleNotZeroComparison(cond, entryLabel, exitLabel);

    codeGen.EmitLocalLabel(exitLabel);
    currFn.labels.pop();
}

void SemanticAnalyzer::ForLoopStatement(const Ast::Node *root)
{
    const Ast::Node* decl = &root->lChild[0];
    const Ast::Node* condNode = &root->lChild[1];
    const Ast::Node* update = &root->lChild[2];
    const Ast::Node* body = &root->lChild[3];
    int64_t bodyLabel = codeGen.GetIdxForLocalVar();
    int64_t exitLabel = codeGen.GetIdxForLocalVar();
    currFn.labels.push(exitLabel);

    symTab->CreateNewScope(Scope::LOCAL);
    Analyze(decl);

    int64_t entryLabel = codeGen.GetIdxForLocalVar();
    codeGen.EmitLocalJump(entryLabel);
    codeGen.EmitLocalLabel(entryLabel);
    if(condNode->type != Ast::none)
    {
        ExprRet cond = AnalyzeExpr(condNode);
        HandleNotZeroComparison(cond, bodyLabel, exitLabel);
    }
    else
    {
        codeGen.EmitLocalJump(bodyLabel);
    }
    codeGen.EmitLocalLabel(bodyLabel);
    Analyze(body);
    AnalyzeExpr(update);
    codeGen.EmitLocalJump(entryLabel);
    codeGen.EmitLocalLabel(exitLabel);

    symTab->PopScope();
    currFn.labels.pop();

}

void SemanticAnalyzer::BreakStatement(const Ast::Node *root)
{
    if(currFn.labels.size()==0)
    {
        IssueWarning(&root->token, "'break' cannot be used outside of loops");
    }
    codeGen.EmitLocalJump(currFn.labels.top());
    currFn.breakCalled = true;
}

void SemanticAnalyzer::LabelStatement(const Ast::Node *root)
{
    int64_t labelId = 0;
    std::string_view lableName = GetViewForToken(root->token, manager);
    const auto label = currFn.namedLabels.find(lableName);
    if(label != currFn.namedLabels.cend() && label->second >= 0)
    {
        IssueWarning(&root->token, "Label name redefinition")
    }
    else if(label != currFn.namedLabels.cend() && label->second < 0)
    {
        labelId = label->second * -1;
    }
    else
    {
        labelId = codeGen.GetIdxForLocalVar();
    }

    codeGen.EmitLocalJump(labelId);
    codeGen.EmitLocalLabel(labelId);
    currFn.namedLabels[lableName] = labelId;
    Analyze(root->lChild);
}

void SemanticAnalyzer::GotoStatement(const Ast::Node *root)
{
    std::string_view lableName = GetViewForToken(root->token, manager);
    auto label = currFn.namedLabels.find(lableName);
    if(label == currFn.namedLabels.cend())
    {
        // if idx < 0 that means label is not placed
        currFn.namedLabels[lableName] = codeGen.GetIdxForLocalVar() * -1; 
        label = currFn.namedLabels.find(lableName);
    }
    int64_t labelValue = label->second < 0 ? label->second * -1 : label->second;
    codeGen.EmitLocalJump(labelValue);
}
