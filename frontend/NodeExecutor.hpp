#pragma once
#include "AstNode.hpp"
#include "TypedNumber.hpp"
#include "../utils/Logger.hpp"

struct NodeExecutor
{
    NodeExecutor(FileManager* fm);
    Typed::Number ExecuteNode(Ast::Node* expr);
    const char* GetDataPtr(const Token *token);
    uint8_t GetTokenMode(const Token &token);

    FileManager* fm;
    Logger logger;
};