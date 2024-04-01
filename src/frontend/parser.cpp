#include "../../include/frontend/parser.hpp"

using namespace std;
vector<AstNode*> parse(Scanner& scanner)
{
    vector<AstNode*> instructions;


    while (scanner.getCurrentToken().type != TokenType::END_OF_FILE)
    {
        if(isTypeSpecifier(scanner.getCurrentToken()))
        {
            AstNode* parsedDeclaration = parseDeclaration(scanner);
            if(parsedDeclaration)
            {
                instructions.push_back(parsedDeclaration);
            }
        }
        else
        {
            instructions.push_back(parseExpression(scanner));
            scanner.consume(TokenType::SEMICOLON);
        }
        

    }
    
    return instructions;
}
