#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include "system_v_abi.hpp"
#include "../symtab_utils.hpp"
#include "../frontend/parser.hpp"

#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  8
#define R9  9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15
#define RIP 16
#define FLAGS 17
#define CS 18
#define SS 19
#define DS 20
#define ES 21
#define FS 22
#define GS 23

#define REG_FREE 0x00
#define REG_USED 0x01
#define REG_CALLER_RES 0x02

struct Reg
{
    uint8_t state;
    std::string symbol;
};

struct CpuState
{
    uint64_t frameSize;
    Reg reg[16];
    uint8_t retSignature[2];
};

CpuState* generateCpuState(AstNode* fnDef, SymbolTable* localSymtab, SymbolFunction* symFn);
void bindReturnValue(CpuState* cpu, SymbolTable* localSymtab, SymbolFunction* symFn);
void fillTypeHwdInfo(SymbolTable *localSymtab, SymbolType* symType);
uint8_t getSysVGroup(SymbolType* type);
#endif