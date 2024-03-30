#ifndef CODE_GENERATOR_UTILS
#define CODE_GENERATOR_UTILS

class InstructionBuffer
{
public:
    InstructionBuffer(unsigned int startSize);
private:
    char* buffer;
    unsigned int bufferSize;
    unsigned int sizeInUse;
};





#endif