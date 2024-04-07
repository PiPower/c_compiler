#ifndef CODE_GENERATOR_UTILS
#define CODE_GENERATOR_UTILS
#include <vector>
#include <string>

class InstructionBuffer
{
public:
    InstructionBuffer(unsigned int startSize);
    InstructionBuffer(const InstructionBuffer& src);
    void writeInstruction(const char* instruction);
    void writeInstruction(const InstructionBuffer& instructionbuffer);
    std::string readBackwardUntilHit(unsigned int pos, char stop = ' ');
    void writeAtPos(unsigned int pos,const char* string);
    void bufferForLateBinding(int size);
    bool popBindingData(unsigned int&pos, unsigned int& size);
    void reserveSpace(int len);
    void reallocate();
    unsigned int getSize();
    char* getBuffer();
    ~InstructionBuffer();
private:
    char* buffer;
    unsigned int bufferSize;
    unsigned int sizeInUse;
    std::vector<unsigned int> lateBindingPos;
    std::vector<unsigned int> lateBindingSize;
};


void bprintf(InstructionBuffer* buffer, const char* format, ...);


#endif