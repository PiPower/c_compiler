#include "../../include/frontend/parser.hpp"

using namespace std;
vector<AstNode*> parse(Scanner& scanner)
{
    Token token = scanner.getCurrentToken();
    while (token.type != TokenType::END_OF_FILE)
    {
        if(isTypeSpecifier(token))
        {
            parseDeclaration(scanner, token);
        }
    }
    
}

inline bool isTypeSpecifier(Token &token)
{
    return (int)token.kType >= (int)KeywordType::VOID && (int)token.kType <= (int)KeywordType::_COMPLEX  ;
}

AstNode* parseDeclaration(Scanner& scanner, Token type)
{

}