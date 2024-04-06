#include "../../include/backend/code_generator_utils.hpp"
#include <cstring>
#include <stdarg.h>
#include <cstdlib>
#include <cstdio>
#define SCRATCHPAD_SIZE (1000)
char scratchpad[SCRATCHPAD_SIZE];

InstructionBuffer::InstructionBuffer(unsigned int startSize)
:
bufferSize(startSize), sizeInUse(0)
{
    buffer = new char[startSize];
}

InstructionBuffer::InstructionBuffer(const InstructionBuffer &src)
{
    buffer = new char[src.bufferSize];
    sizeInUse = src.sizeInUse;
    bufferSize = src.bufferSize;
    memcpy(buffer, src.buffer, sizeInUse);
}
void InstructionBuffer::writeInstruction(const char *instruction)
{
    unsigned int len = strlen(instruction);

    while(sizeInUse + len > bufferSize)
    {
        reallocate();
    }
    memcpy(buffer + sizeInUse, instruction, len);
    sizeInUse += len;
}
void InstructionBuffer::writeInstruction(const InstructionBuffer &instructionbuffer)
{
    int len = instructionbuffer.sizeInUse;
    while(sizeInUse + len > bufferSize)
    {
        reallocate();
    }
    memcpy(buffer + sizeInUse, instructionbuffer.buffer, len);
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

InstructionBuffer::~InstructionBuffer()
{
    if(buffer != nullptr)
    {
        delete[] buffer;
        buffer = nullptr;
    }
}

void bprintf(InstructionBuffer *buffer, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int code = vsnprintf(scratchpad, SCRATCHPAD_SIZE, format, args);
    if(!(code > 0  && code < SCRATCHPAD_SIZE ))
    {
        fprintf(stdout, "scratchpad not big enough\n");
        exit(-1);
    }
    
    buffer->writeInstruction(scratchpad);

    va_end(args);
}