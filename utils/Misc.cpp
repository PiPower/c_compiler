#include "Misc.hpp"

bool IsPointer(const AccessType *acc)
{
    if(acc->type == ACC_NONE)
    {
        return false;
    }
    
    const AccessType* currAcc = acc;
    while (currAcc)
    {
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

bool IsArray(const AccessType *acc)
{
        if(acc->type == ACC_NONE)
    {
        return false;
    }
    
    const AccessType* currAcc = acc;
    while (currAcc)
    {
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
