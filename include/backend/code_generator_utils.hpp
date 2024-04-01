#ifndef CODE_GENERATOR_UTILS
#define CODE_GENERATOR_UTILS

class InstructionBuffer
{
public:
    InstructionBuffer(unsigned int startSize);
    void writeInstruction(const char* instruction);
    void writeInstruction(const InstructionBuffer& instructionbuffer);
    void reallocate();
    unsigned int getSize();
    char* getBuffer();
    ~InstructionBuffer();
private:
    char* buffer;
    unsigned int bufferSize;
    unsigned int sizeInUse;
};





#endif