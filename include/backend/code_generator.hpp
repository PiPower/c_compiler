#ifndef CODE_GEN
#define CODE_GEN
#include "./code_generator_utils.hpp"
#include "../frontend/parser.hpp"
#define UNDEFINED_ENTRY (-1)
#define FUNCTION_DEFINED (10)
#define FUNCTION_DECLARED (9)
#define NO_REGISTER (-10)
#define scratchpadSize (1000)

struct FunctionInfo
{
    int arity;
    //left to right order
    std::vector<DataType> argsType;
};

struct SymbolEntry
{
    // means entry is newly created, used to check if entry existed before called,
    // must be changed after creation
    //used if symbol is local variable
    int stackOffset = 0;
    //if FunctionInfo != nullptr symbol represents function
    FunctionInfo* functionInfo;
    int status = UNDEFINED_ENTRY;
};

typedef std::unordered_map<std::string, SymbolEntry> SymbolTable;

struct CompilationState
{
    SymbolTable globalSymTab;
    SymbolTable localSymTab;
    bool insideFunction = false;
    unsigned int stackSize = 0;
};

int translateDeclaration(InstructionBuffer& buffer, AstNode* root);
void freeRegister(int reg);
int allocateRegister(int base = 0 );


InstructionBuffer generateCode(std::vector<AstNode*>& instructionSequence);
int translate(InstructionBuffer& buffer, AstNode* root);
int loadIdentifier(InstructionBuffer& buffer, AstNode* root);
int translateAssignment(InstructionBuffer& buffer, AstNode* root);
void resolveFunctionBinding(InstructionBuffer& buffer);
#endif