#include "../../include/frontend/parser.hpp"

using namespace std;
vector<AstNode*> parse(Scanner& scanner)
{
    vector<AstNode*> instructions;


    while (scanner.getCurrentToken().type != TokenType::END_OF_FILE)
    {
        if(isTypeSpecifier(scanner.getCurrentToken()))
        {
            instructions.push_back(parseDeclaration(scanner));
        }
        else
        {
            instructions.push_back(parseExpression(scanner));
        }
        

    }
    
    return instructions;
}
