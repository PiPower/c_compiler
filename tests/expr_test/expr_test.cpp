#include <stdio.h>
#include <cstdlib>
#include <errno.h>
#include <string.h>
#include <vector>
#include "../../frontend/node_allocator.hpp"
#include "../../frontend/parser.hpp"
#include <iostream>

using  namespace std;

const char* nodeStr[]= {
    //basic math ops
    "*", "/", "%",
    "+", "-", "<<", ">>",
    "&", "^", "|", "~",
// prefix ops
    "++", "++", "--", "--",
//logical ops    
    "&&", "||", "!",
// comparisons
    "<", ">", "<=", 
    ">=", "==", "!=",
// assignment nodes 
    "=", "*=", "/=", "%=",
    "+=", "-=", "<<=", ">>=",
    "&=", "^=", "|=",
};

const char* loadFile(const char* filename);
void test_parsing(const char* file);
string print_parse_tree(AstNode* root);

int main(int agrc, char* args[])
{
    if (agrc != 2 )
    {
        printf("Incorrect number of arguments\n");
        exit(-1);
    }

    const char* file = loadFile(args[agrc -1 ]);
    test_parsing(file);
}

const char* loadFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("%s",  strerror(errno));
        exit(-1);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", filename);
        exit(-2);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", filename);
        exit(-3);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void test_parsing(const char *file)
{
    SymbolTable symtab;
    NodeAllocator allocator;
    Scanner scanner(file);

    symtab.symbols["int8"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 1};
    symtab.symbols["int16"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 2};
    symtab.symbols["int32"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 4};
    symtab.symbols["int64"] = (Symbol*)new SymbolType{SymbolClass::TYPE, true, 8};

    vector<AstNode*> expressions = parse(&scanner, &symtab, &allocator);
    for(AstNode* expr : expressions)
    {
        if(expr && expr->nodeType != NodeType::DECLARATION_LIST)
        {
            cout << print_parse_tree(expr) << endl;
        }
    }
}

string print_parse_tree(AstNode *root)
{
    vector<string> childStrings;
    for(AstNode* child : root->children)
    {
        if(child)
        {
            childStrings.push_back( print_parse_tree(child) );
        }
    }

    if(root->nodeType == NodeType::CONSTANT || root->nodeType == NodeType::IDENTIFIER)
    {
        return *(string*)root->data;
    }

    int typeValue = (int)root->nodeType;
    constexpr int lowerMathOp = (int)NodeType::MULTIPLY;
    if(typeValue >= (int)NodeType::MULTIPLY && typeValue <= (int)NodeType::COMPLIMENT)
    {
        if(childStrings.size() != 2)
        {
            cerr << "Incorrect number of children " << endl;
            exit(-1);
        }
        return "(" + childStrings[0] + " " + nodeStr[typeValue - (int)NodeType::MULTIPLY]+ " " + childStrings[1] + ")";
    }
    else if(typeValue >= (int)NodeType::ASSIGNMENT && typeValue <= (int)NodeType::OR_ASSIGNMENT)
    {
        if(childStrings.size() != 2)
        {
            cerr << "Incorrect number of children " << endl;
            exit(-1);
        }
        return "(" + childStrings[0] + " " + nodeStr[24 + typeValue - (int)NodeType::ASSIGNMENT]+ " " + childStrings[1] + ")";
    }
    return "";
}
