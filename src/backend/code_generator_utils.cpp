#include "../../include/backend/code_generator_utils.hpp"
#include <cstring>
#include <stdarg.h>
#include <cstdlib>
#include <cstdio>
#include <algorithm>

#define SCRATCHPAD_SIZE (1000)
char scratchpad[SCRATCHPAD_SIZE];
using namespace std;

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

void InstructionBuffer::writeAtPos(unsigned int pos,const char* string)
{
    if( pos > sizeInUse)
    {
        printf("Incorrect position variable for overwrite\n");
        exit(-1);
    }

    if( pos + strlen(string) > sizeInUse)
    {
        printf("string too large to be written into buffer\n");
        exit(-1);
    }

    memcpy(buffer + pos, string, strlen(string));
}

void InstructionBuffer::writeInstruction(const InstructionBuffer &instructionbuffer)
{
    int len = instructionbuffer.sizeInUse;
    while(sizeInUse + len > bufferSize)
    {
        reallocate();
    }
    memcpy(buffer + sizeInUse, instructionbuffer.buffer, len);
    for(int i=0; i < instructionbuffer.lateBindingPos.size(); i++)
    {
        lateBindingPos.push_back(sizeInUse + instructionbuffer.lateBindingPos[i] );
        lateBindingSize.push_back( instructionbuffer.lateBindingSize[i]);
    }
    sizeInUse += len;
}

void InstructionBuffer::bufferForLateBinding(int size)
{
    lateBindingPos.push_back(sizeInUse);
    lateBindingSize.push_back(size);
    reserveSpace(size);
}
bool InstructionBuffer::popBindingData(unsigned int &pos, unsigned int &size)
{
    if( lateBindingPos.size() == 0 )
    {
        return false;
    }

    pos =  lateBindingPos[lateBindingPos.size() - 1];
    lateBindingPos.pop_back();
    size = lateBindingSize[lateBindingSize.size() - 1];
    lateBindingSize.pop_back();
    return true;
}

void InstructionBuffer::reserveSpace(int len)
{
    while(sizeInUse + len > bufferSize)
    {
        reallocate();
    }
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

std::string InstructionBuffer::readBackwardUntilHit(unsigned int pos, char stop)
{
    string out = "";

    if(pos < 0 || pos > sizeInUse)
    {
        fprintf(stdout, "Incorrect position index into buffer \n");
        exit(-1);
    }

    while (buffer[pos] != stop && pos >= 0 )
    {
        out+= buffer[pos];
        pos--;
    }
    reverse(out.begin(), out.end());
    return out;
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