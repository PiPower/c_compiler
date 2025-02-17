#include "parser.hpp"
#include "parser_internal.hpp"
#define ERR_BUFF_SIZE 10000

using namespace std;
std::vector<AstNode *> parse(Scanner *scanner, SymbolTable *symtab, NodeAllocator *allocator)
{
    ParserState parser;
    parser.allocator = allocator;
    parser.scanner = scanner;
    parser.symtab = symtab;
    parser.isParsingAssignment = false;
    parser.jmpHolder = nullptr;
    parser.errorMessage = new char[ERR_BUFF_SIZE];
    parser.errorMessageLen = ERR_BUFF_SIZE;
    parser.errCode = 0;

    if( setjmp(parser.jmpBuff) != 0)
    {
        freeAllNodes(parser.allocator);
        printf("%s", parser.errorMessage);
        exit(-1);
        return {nullptr};
    }
    
    vector<AstNode *> statements; 
    while (parser.scanner->peekToken().type != TokenType::END_OF_FILE)
    {
        while (TOKEN_MATCH(&parser, TokenType::SEMICOLON)){}//clear floating semicolons
        AstNode* tree = parseDeclaration(&parser);
        if(tree == PARSER_SUCC)
        {
            continue; //empty decl call, needs to parse next line
        }
        vector<AstNode *> processedTree = processDeclarationTree(tree, &parser);
        if( tree->nodeType == NodeType::FUNCTION_DEF)
        {
            if( tree != processedTree[0])
            {
                printf("Something went wrong for processing function definition\n");
                exit(-1);
            }
            AstNode* functionBody = parseFunctionBody(&parser, tree);
            tree->children[0] = functionBody;
        }
        if(processedTree.size() > 0)
        {
            statements.insert(statements.end(), processedTree.begin(), processedTree.end());
        }
    }
    return statements;
}


