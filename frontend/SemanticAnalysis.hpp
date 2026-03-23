#pragma once
#include "AstNode.hpp"

struct SemanticAnalyzer
{
    void Analyze(const Ast::Node* root);
    bool AliaseOfType(const std::string_view identifier);
};

