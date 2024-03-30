#include "../../include/backend/code_generator_utils.hpp"

InstructionBuffer::InstructionBuffer(unsigned int startSize)
:
bufferSize(startSize), sizeInUse(0)
{
    buffer = new char[10000];
}