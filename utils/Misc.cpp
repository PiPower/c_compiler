#include "Misc.hpp"
#include "../frontend/TypedNumber.hpp"
#define IssueWarning(tokenPtr, errorMsg, ...) logger->IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

ArraySize GetArrayElemCount(AccessArray *accArray, Logger* logger, NodeExecutor* ne)
{
    bool hitPointer = false;
    bool hitArray = false;
    int64_t arrayCount = 1;

    AccessType* accType = accArray->ptr;
    for(size_t i = 0; i < accArray->count; i++)
    {
        if(accType[i].type == ACC_POINTER)
        {
            hitPointer = true;
            break;
        }
        else if(accType[i].type == ACC_ARRAY)
        {
            hitArray = true;
            arrayCount *= accType[i].array.size;
        }
        else if(ACC_ARRAY_VLA)
        {
            if(hitArray)
            {
                IssueWarning(nullptr, "Variable length array can be only in first dimension")
            }

            Typed::Number num = ne->ExecuteNode(accType[i].array.asmExpr);
            if(num.type == Typed::d_float || num.type == Typed::d_double)
            {
                IssueWarning(nullptr, "Floats cannot be used inside array size declaration")
            }
            arrayCount *= num.int64;
        }
    }

    return {arrayCount, hitPointer};
}

MemoryDesc GetMemoryDesc(AccessArray *accArray, SymbolType *type, Logger *logger, NodeExecutor *ne)
{
    ArraySize arrSize = GetArrayElemCount(accArray, logger, ne);
    if(arrSize.hitPointer)
    {
        return {arrSize.elementCount * 8, 8};
    }
   return {arrSize.elementCount * type->size, type->alignment};
}

bool IsPointer(const AccessArray *accArray, size_t startIdx )
{
    for(size_t i = startIdx; i < accArray->count; i++)
    {
        const AccessType* currAcc = &accArray->ptr[i];
        if(currAcc->type == ACC_POINTER)
        {
            return true;
        }
        else if(currAcc->type == ACC_ARRAY)
        {
            // do nothing 
        }
        else if(currAcc->type == ACC_FN_CALL)
        {
            return false;
        }
        else if(currAcc->type == ACC_FN_DECL)
        {
            return false;
        }
        currAcc = currAcc->next;
    }

    return false;
}

bool IsArray(const AccessArray *accArray)
{

    for(size_t i = 0; i < accArray->count; i++)
    {
        const AccessType* currAcc = &accArray->ptr[i];
        if(currAcc->type == ACC_POINTER)
        {
            return false;
        }
        else if(currAcc->type == ACC_ARRAY)
        {
            return true;
        }
        else if(currAcc->type == ACC_FN_CALL)
        {
            return false;
        }
        else if(currAcc->type == ACC_FN_DECL)
        {
            return false;
        }
        currAcc = currAcc->next;
    }

    return false;
}
