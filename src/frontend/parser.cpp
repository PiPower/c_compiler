#include "../../include/frontend/parser.hpp"

using namespace std;
vector<AstNode*> parse(Scanner& scanner)
{
    vector<AstNode*> instructions;


    while (scanner.getCurrentToken().type != TokenType::END_OF_FILE)
    {
       AstNode* stmt = parseStatementAndDeclaration(scanner);
       if(stmt)
       {
            instructions.push_back(stmt);
       }
    }
    
    return instructions;
}
