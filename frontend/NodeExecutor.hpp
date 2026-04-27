#pragma once
#include "AstNode.hpp"
#include "TypedNumber.hpp"
#include "../utils/Logger.hpp"
#include "../SymbolTable.hpp"

struct SemanticAnalyzer;

struct AccessDesc
{
    bool hitPtr;
    size_t arraySize;
};

struct NodeExecutor
{
    NodeExecutor(FileManager* fm, SemanticAnalyzer* sm = nullptr);
    AccessDesc parseAccess(const AccessType* accType);
    Typed::Number ExecuteNode(const Ast::Node* expr);
    const char* GetDataPtr(const Token *token);
    uint8_t GetTokenMode(const Token &token);

    FileManager* fm;
    Logger logger;
    SemanticAnalyzer* sema;
};