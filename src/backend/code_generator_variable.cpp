#include "../../include/backend/code_generator.hpp"

extern const char* registers[] ;
extern const char* registersByte[];
extern bool allocatedFlag[];
extern CompilationState compilationState;
using namespace std;

static int loadIdentifierFromGlobalScope(InstructionBuffer& buffer, AstNode* root)
{   

    SymbolEntry& entry = compilationState.globalSymTab[STRING(root)];
    if(entry.status == UNDEFINED_ENTRY)
    {
        fprintf(stdout, "trying to refrence undefined variable\n");
        exit(-1);
    }

    int r = allocateRegister();
    bprintf(&buffer,"\tmovq %s(%%rip), %s\n", STRING(root).c_str(), registers[r]);
    return r;
}
int loadIdentifierInFunction(InstructionBuffer& buffer, AstNode* root)
{
    auto iter = compilationState.localSymTab.find(STRING(root));
    if(iter == compilationState.localSymTab.end())
    {
        return  loadIdentifierFromGlobalScope(buffer, root);
    }

    SymbolEntry& entry = iter->second;
    int r = allocateRegister();
    int offset = entry.stackOffset;
    bprintf(&buffer, "\tmovq %d(%%rbp), %s\n", offset, registers[r]);
    return r;
}


int loadIdentifier(InstructionBuffer& buffer, AstNode* root)
{
    if(compilationState.insideFunction)
    {
        return loadIdentifierInFunction(buffer, root);
    }
    return loadIdentifierFromGlobalScope(buffer, root);

}

static void declareLocalVariable(InstructionBuffer& buffer, AstNode* declaration)
{
    AstNode* identifier = declaration->children[0];
    SymbolEntry& entry = compilationState.localSymTab[STRING(identifier)];

    if(entry.status != UNDEFINED_ENTRY)
    {
        fprintf(stdout, "redeclaration of local variable %s\n", STRING(identifier).c_str());
        exit(-1);
    }

    entry.status = 0;
    entry.functionInfo = nullptr;
    compilationState.stackOffset += 8;
    entry.stackOffset = -compilationState.stackOffset;
    if(declaration->children.size() > 1)
    {   
        int r = translate(buffer, declaration->children[1]);
        bprintf(&buffer, "\tmovq %s, %d(%%rbp)\n", registers[r], entry.stackOffset);
        freeRegister(r);
    }
}

static void declareGlobalVariable(InstructionBuffer& buffer, AstNode* declaration)
{
  static const char* defTemplate = "\t.data\n" 
                                    "\t.globl %s\n"
                                     "%s:\n";

    AstNode* identifier = declaration->children[0];
    SymbolEntry& entry = compilationState.globalSymTab[STRING(identifier)];

    if(entry.status != UNDEFINED_ENTRY)
    {
        fprintf(stdout, "redeclaration of global variable %s\n", STRING(identifier).c_str());
        exit(-1);
    }

    entry.status = 0;
    entry.functionInfo = nullptr;
    
    bprintf(&buffer, defTemplate, STRING(identifier).c_str(), STRING(identifier).c_str());
    if(declaration->children.size() > 1)
    {   
        if(declaration->children[1]->nodeType == NodeType::CONSTANT)
        {
            bprintf(&buffer,  "\t.quad %ld\n", declaration->children[1]->context.int_32);
        }
        else
        {
            fprintf(stdout, "unsupported initialization mode");
            exit(-1);
        }
        // TODO evaluate if given expression is constant 
    }
    else
    {
        buffer.writeInstruction("\t.zero 8\n");
    }
}


static void declareVariable(InstructionBuffer& buffer, AstNode* declaration)
{

    if(compilationState.insideFunction)
    {
        return declareLocalVariable(buffer, declaration);
    }
    else
    {
        return declareGlobalVariable(buffer, declaration);
    }

}

FunctionInfo* createFunctionInfo(AstNode* args)
{
    FunctionInfo* info = new FunctionInfo();
    info->arity = args->children.size();
    for(int i=0; i < info->arity; i++)
    {
        info->argsType.push_back(DataType::INT_64);
    }
    return info;
}

static void declareFunction(InstructionBuffer& buffer, AstNode* declaration)
{
                                
    AstNode* identifier = declaration->children[0];
    SymbolEntry& entry = compilationState.globalSymTab[STRING(identifier)];

    if(entry.status == FUNCTION_DEFINED && declaration->nodeType == NodeType::FUNCTION_DEFINITION)
    {
        fprintf(stdout, "redefinition of function  %s\n", STRING(identifier).c_str());
        exit(-1);
    }

    if( entry.status != FUNCTION_DECLARED)
    {
        entry.status = FUNCTION_DECLARED;
        entry.functionInfo = createFunctionInfo(declaration->children[1]);
    }

    if(declaration->children.size() == 3)
    {
        static const char* preambule = "\t.text\n" "\t.globl %s\n""%s:\n"
                                        "\tpushq %rbp\n" "\tmovq %rsp, %rbp\n""\tsubq $%d, %%rsp\n";
        static const char* footer = "\tmovq %rbp, %rsp\n" "\tpopq %rbp\n" "\tret\n";
        entry.status = FUNCTION_DEFINED;
        compilationState.insideFunction = true;
        compilationState.stackSize = declaration->context.int_64;
        compilationState.stackOffset = 0;

        InstructionBuffer functionBuffer(1000);
        freeRegister( translate( functionBuffer, declaration->children[2]  ) );

        bprintf(&buffer, preambule , STRING(identifier).c_str(), STRING(identifier).c_str(), compilationState.stackSize);
        buffer.writeInstruction(functionBuffer);
        buffer.writeInstruction(footer);

        compilationState.localSymTab.clear();
        compilationState.insideFunction = false;
        compilationState.stackSize = 0;
        compilationState.stackOffset = 0;
    }


}

int translateDeclaration(InstructionBuffer& buffer, AstNode* root)
{

    for(AstNode* declaration : root->children)
    {

        switch (declaration->nodeType)
        {
        case NodeType::FUNCTION_DECLARATION:
        case NodeType::FUNCTION_DEFINITION:
            declareFunction(buffer, declaration);
            break;
        case NodeType::DEFINITION_VARIABLE:
        case NodeType::DECLARATION_VARIABLE:
            declareVariable(buffer, declaration);
            break;
        default:
            fprintf(stdout, "unsupported ast node for declaration");
            exit(-1);
            break;
        }

    }

    return NO_REGISTER;
}

int translateGlobalAssignment(InstructionBuffer& buffer, AstNode* identifier, int reg)
{
    static const char* globalAssignmentInstruction = "\tmovq %s, %s(%rip)\n";
    SymbolEntry& entry = compilationState.globalSymTab[STRING(identifier)];
    if(entry.status == UNDEFINED_ENTRY)
    {
        fprintf(stdout, "trying to refrence undefined variable\n");
        exit(-1);
    }

    bprintf(&buffer, globalAssignmentInstruction, registers[reg], STRING(identifier).c_str() );
    return reg;
}

int translateLocalAssignment(InstructionBuffer& buffer, AstNode* identifier, int reg)
{
    static const char* localAssignmentInstruction = "\tmovq %s, %d(%rbp)\n";

    auto iter = compilationState.localSymTab.find(STRING(identifier));
    if(iter == compilationState.localSymTab.end())
    {
        return  translateGlobalAssignment(buffer, identifier, reg);
    }

    bprintf(&buffer, localAssignmentInstruction, registers[reg], iter->second.stackOffset );
    return reg;
}

int translateAssignment(InstructionBuffer& buffer, AstNode* root)
{
    int reg = translate(buffer, root->children[1]);
    if(compilationState.insideFunction)
    {
        return translateLocalAssignment(buffer, root->children[0], reg);
    }

    return translateGlobalAssignment(buffer, root->children[0], reg);
}

void resolveFunctionBinding(InstructionBuffer& buffer)
{
    unsigned int pos;
    unsigned int size;
    while (buffer.popBindingData(pos, size))
    {
        string identifier = buffer.readBackwardUntilHit(pos - 1);

        SymbolEntry& entry = compilationState.globalSymTab[identifier];
        if(entry.status == FUNCTION_DECLARED) 
        {
            buffer.writeAtPos(pos, "@PLT\n");
        }
        else if(entry.status == FUNCTION_DEFINED) 
        {
            buffer.writeAtPos(pos, "    \n");
        }
    }
    
}