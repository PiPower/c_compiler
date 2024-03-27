#include "../include/compiler.hpp"
#include "../include/frontend/parser.hpp"

using namespace std;

void compile(const char *sourceCode)
{   
    Scanner scanner(sourceCode);
    vector<AstNode*> instructionSequence = parse(scanner);
}