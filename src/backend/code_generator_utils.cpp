#include "../../include/backend/code_generator_utils.hpp"
#include <cstring>

InstructionBuffer::InstructionBuffer(unsigned int startSize)
:
bufferSize(startSize), sizeInUse(0)
{
    buffer = new char[startSize];
}

void InstructionBuffer::writeInstruction(const char* instruction)
{
    unsigned int len = strlen(instruction);
    
    if(sizeInUse + len > bufferSize)
    {
        reallocate();
    }
    memcpy(buffer + sizeInUse, instruction, len);
    sizeInUse += len;
}
void InstructionBuffer::reallocate()
{
    bufferSize = bufferSize * 1.5;
    char* newMem = new char[bufferSize];
    memcpy(newMem, buffer, sizeInUse);
    delete buffer;
    buffer = newMem;
}
unsigned int InstructionBuffer::getSize()
{
    return sizeInUse;
}
char *InstructionBuffer::getBuffer()
{
    return buffer;
}