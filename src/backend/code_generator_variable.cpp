#include "../../include/backend/code_generator.hpp"

extern const char* registers[] ;
extern const char* registersByte[];
extern char scratchpad[];
extern bool allocatedFlag[];
extern CompilationState compilationState;
using namespace std;

static int loadIdentifierFromGlobalScope(InstructionBuffer& buffer, AstNode* root)
{   

    SymbolEntry& entry = compilationState.globalSymTab[STRING(root)];
    if(entry.x == UNDEFINED_ENTRY)
    {
        fprintf(stdout, "trying to refrence undefined variable\n");
        exit(-1);
    }

    int r = allocateRegister();
    snprintf(scratchpad, scratchpadSize, "\tmovq %s(%%rip), %s\n", STRING(root).c_str(), registers[r]);
    buffer.writeInstruction(scratchpad);
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
    snprintf(scratchpad, scratchpadSize, "\tmovq %d(%%rbp), %s\n", offset, registers[r]);
    buffer.writeInstruction(scratchpad);
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

    if(entry.x != UNDEFINED_ENTRY)
    {
        fprintf(stdout, "redeclaration of local variable %s\n", STRING(identifier).c_str());
        exit(-1);
    }

    entry.x = 0;
    entry.functionInfo = nullptr;
    compilationState.stackSize += 8;
    entry.stackOffset = -compilationState.stackSize;
    if(declaration->children.size() > 1)
    {   
        int r = translate(buffer, declaration->children[1]);
        snprintf(scratchpad, scratchpadSize, "\tmovq %s, %d(%%rbp)\n", registers[r], entry.stackOffset);
        buffer.writeInstruction(scratchpad);

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

    if(entry.x != UNDEFINED_ENTRY)
    {
        fprintf(stdout, "redeclaration of global variable %s\n", STRING(identifier).c_str());
        exit(-1);
    }

    entry.x = 0;
    entry.functionInfo = nullptr;
    
    snprintf(scratchpad, scratchpadSize, defTemplate, STRING(identifier).c_str(), STRING(identifier).c_str());
    buffer.writeInstruction(scratchpad);
    if(declaration->children.size() > 1)
    {   
        if(declaration->children[1]->nodeType == NodeType::CONSTANT)
        {
            snprintf(scratchpad, scratchpadSize, "\t.quad %ld\n", declaration->children[1]->context.int_32);
            buffer.writeInstruction(scratchpad);
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
        snprintf(scratchpad, scratchpadSize,  "\t.zero 8\n");
        buffer.writeInstruction(scratchpad);
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
    static const char* defTemplate = "\t.text\n" 
                                     "\t.globl %s\n"
                                     "%s:\n";
                                    
    AstNode* identifier = declaration->children[0];
    SymbolEntry& entry = compilationState.globalSymTab[STRING(identifier)];

    if(entry.x == FUNCTION_DEFINED && declaration->nodeType == NodeType::FUNCTION_DEFINITION)
    {
        fprintf(stdout, "redefinition of function  %s\n", STRING(identifier).c_str());
        exit(-1);
    }

    entry.x = FUNCTION_DECLARED;
    entry.functionInfo = createFunctionInfo(declaration->children[1]);
    if(declaration->children.size() == 3)
    {
        entry.x = FUNCTION_DEFINED;
        compilationState.insideFunction = true;
        compilationState.stackSize = 0;

        snprintf(scratchpad, scratchpadSize, defTemplate , STRING(identifier).c_str(), STRING(identifier).c_str());
        buffer.writeInstruction(scratchpad);
        freeRegister( translate( buffer, declaration->children[2]  ) );

        compilationState.localSymTab.clear();
        compilationState.insideFunction = false;
        compilationState.stackSize = 0;
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
    if(entry.x == UNDEFINED_ENTRY)
    {
        fprintf(stdout, "trying to refrence undefined variable\n");
        exit(-1);
    }


    snprintf(scratchpad, scratchpadSize, globalAssignmentInstruction, registers[reg], STRING(identifier).c_str());
    buffer.writeInstruction(scratchpad);
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

    snprintf(scratchpad, scratchpadSize, localAssignmentInstruction, registers[reg], iter->second.stackOffset);
    buffer.writeInstruction(scratchpad);
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