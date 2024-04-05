#include "../../include/backend/code_generator.hpp"

extern const char* registers[] ;
extern const char* registersByte[];
extern char scratchpad[];
extern SymbolTable symTab;

using namespace std;

static void declareVariable(InstructionBuffer& buffer, AstNode* declaration)
{
    static const char* defTemplate = "\t.data\n" 
                                    "\t.globl %s\n"
                                     "%s:\n";

    AstNode* identifier = declaration->children[0];
    SymbolEntry& entry = symTab[STRING(identifier)];

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
        /*
        snprintf(scratchpad, scratchpadSize, "\t.text\n""static_init_var_%s:\n", STRING(identifier).c_str());
        buffer.writeInstruction(scratchpad);
        int reg = translate(  buffer, declaration->children[1]);
        snprintf(scratchpad, scratchpadSize, "\tmovq %s, %s(%%rip)\n", registers[reg], STRING(identifier).c_str());
        buffer.writeInstruction(scratchpad);
        freeRegister(reg);
        */
    }
    else
    {
        snprintf(scratchpad, scratchpadSize,  "\t.zero 8\n");
        buffer.writeInstruction(scratchpad);
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
    SymbolEntry& entry = symTab[STRING(identifier)];

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
        snprintf(scratchpad, scratchpadSize, defTemplate , STRING(identifier).c_str(), STRING(identifier).c_str());
        buffer.writeInstruction(scratchpad);
        freeRegister( translate( buffer, declaration->children[2]  ) );
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
