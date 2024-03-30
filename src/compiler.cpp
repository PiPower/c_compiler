#include "../include/compiler.hpp"
#include "../include/frontend/parser.hpp"
#include "../include/backend/code_generator.hpp"
#include <fstream>
#include <string.h>

using namespace std;

void compile(const char *sourceCode, const char* outfileName)
{   
    fstream outfile{outfileName,  ios_base::binary | ios_base::out};
    if(!outfile.good())
    {
        fprintf(stdout, "ERROR: %s \n", strerror(errno));
        exit(-1);
    }

    Scanner scanner(sourceCode);
    vector<AstNode*> instructionSequence = parse(scanner);
    InstructionBuffer instructionPtr = generateCode(instructionSequence);
    outfile.write(instructionPtr.getBuffer(), instructionPtr.getSize());
    outfile.close();
}