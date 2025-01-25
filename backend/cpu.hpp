#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include "system_v_abi.hpp"
#include "../symtab_utils.hpp"
#include "../frontend/parser.hpp"

#define IDX_XMM 2
#define IDX_YMM 1
#define IDX_ZMM 0

#define IDX_R8HI 4
#define IDX_R8LO 3
#define IDX_R16 2
#define IDX_R32 1
#define IDX_R64 0


#define RAX 0
#define RBX 1
#define RCX 2
#define RDX 3
#define RSI 4
#define RDI 5
#define RSP 6
#define RBP 7
#define R8  8
#define R9  9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

#define XMM0 16
#define XMM1 17
#define XMM2 18
#define XMM3 19
#define XMM4 20
#define XMM5 21
#define XMM6 22
#define XMM7 23
#define XMM8 24
#define XMM9 25
#define XMM10 26
#define XMM11 27
#define XMM12 28
#define XMM13 29
#define XMM14 30
#define XMM15 31

#define RIP 32
#define FLAGS 33
#define CS 34
#define SS 35
#define DS 36
#define ES 37
#define FS 38
#define GS 39

#define REG_FREE 0x00
#define REG_USED 0x01
#define REG_CALLER_RES 0x02
#define REG_FORBIDDEN 0x03
extern const char* cpu_registers_str[][5];

/* 
    state: on of REG_FREE, REG_USED, REG_CALLER_RES
    symbol: name of symbol stack( it might be in symtab but that is not guaranteed!!!!)
*/
struct Reg
{
    uint8_t state;
    std::string symbol;
};

enum class hwdVarType
{
    SIGNED,
    UNSIGNED,
    FLOAT_32,
    FLOAT_64
};

enum class Storage
{
    NONE,
    STACK,
    REG,
};

/*
    Storage == MEMORY -> offset represent offset in memory from rsp on function entry 
    Storage == REG -> offset represent id of register used to store variable
    offset -> if Storage == REG stores ID of assigned register else stores address of the variable 
*/
struct VariableCpuDesc
{
    Storage storageType;
    long int offset;
};

/*
    stack is allowed to be 32 bit long above and below rbp value on entry
    every variable(expect for temporary) will be stored on stack, if there is 
    assginment expression data is ALWAYS to be flushed to stack
*/
struct CpuState
{
    Reg reg[32];
    std::unordered_map<std::string,VariableCpuDesc> stackData;
    std::unordered_map<std::string,VariableCpuDesc> regData;
    uint32_t stackArgsOffset;
    uint32_t currentStackSize;
    uint32_t runtimeStackSize;
    uint32_t maxStackSize;
    uint8_t retSignature[2];
};

CpuState* generateCpuState(AstNode* fnDef, SymbolTable* localSymtab, SymbolFunction* symFn);
void bindReturnValue(CpuState* cpu, SymbolTable* localSymtab, SymbolFunction* symFn);
void bindArgs(CpuState* cpu, SymbolTable* localSymtab, SymbolFunction* symFn);
void bindArg(CpuState* cpu, SymbolVariable* symVar, SymbolType* symType, const std::string& varname, SysVgrDesc cls);
void reserveCalleSavedRegs(CpuState* cpu);
void bindBlockToStack(CpuState* cpu, SymbolTable* localSymtab);
void fillTypeHwdInfo(SymbolTable *localSymtab, SymbolType* symType);
SysVgrDesc tryPackToRegisters(SymbolTable *localSymtab, SymbolType* symType);
uint8_t is8ByteAligned(SymbolTable *localSymtab, SymbolType* symType);
SysVgrDesc getSysVclass(SymbolTable *localSymtab, SymbolType* type);
uint8_t resolveSysVclass(uint8_t cl1, uint8_t cl2);
void bindArgToCpuStack(CpuState* cpu, SymbolType* symType, const std::string& varname);
char getUnusedArgRegId(CpuState* cpu);
char getUnusedArgMMRegId(CpuState* cpu);
void fillTypeHwdInfoForBlock(SymbolTable* localSymtab);
void increaseStackSize(CpuState* cpu, int64_t size);
std::string getCpuRegStr(uint8_t regIdx, uint8_t regSize);
VariableCpuDesc fetchVariable(const CpuState* cpu, const std::string& varName);
std::string generateLocalConstantLabel();
bool isRegisterUsed(const CpuState* cpu, uint8_t regIdx);
bool registerStores(const CpuState* cpu, uint8_t regIdx, const std::string& varName);
#endif