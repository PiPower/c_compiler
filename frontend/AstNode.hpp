#pragma once
#include "Lexer.hpp"

#define NODE_TYPES \
    X(none) \
    X(identifier) \
    X(constant) \
    X(declaration) \
    X(string_literal) \
    X(character) \
    X(expression) \
    X(cond_expression) \
    /* Basic math ops */ \
    OP(multiply) \
    OP(divide) \
    OP(divide_modulo) \
    OP(add) \
    OP(subtract) \
    OP(l_shift) \
    OP(r_shift) \
    OP(and) \
    OP(exc_or) \
    OP(inc_or) \
    OP(complement) \
    /* Prefix ops */ \
    OP(pre_inc) \
    OP(post_inc) \
    OP(pre_dec) \
    OP(post_dec) \
    OP(plus) \
    OP(minus) \
    /* Logical ops */ \
    OP(log_and) \
    OP(log_or) \
    OP(log_not) \
    /* Comparisons */ \
    OP(less) \
    OP(greater) \
    OP(less_equal) \
    OP(greater_equal) \
    OP(equal) \
    OP(not_equal) \
    OP(log_negate) \
    /* Assignment nodes */ \
    X(assignment) \
    X(mul_assignment) \
    X(div_assignment) \
    X(mod_assignment) \
    X(add_assignment) \
    X(sub_assignment) \
    X(l_shift_assignment) \
    X(r_shift_assignment) \
    X(and_assignment) \
    X(exc_or_assignment) \
    X(inc_or_assignment) \
    /* Function */ \
    X(function_decl) \
    X(function_def) \
    X(args_expr_list) \
    X(function_call) \
    X(parameter_list) \
    X(identifier_list) \
    /* Statement (including loops and conditionals) */ \
    ST(block) \
    ST(if) \
    ST(else) \
    ST(for_loop) \
    ST(while_loop) \
    ST(do_while_loop) \
    ST(return) \
    ST(continue) \
    ST(break) \
    /* Declarations*/ \
    X(declaration_specifier) \
    X(storage_specifier) \
    X(type_qualifier) \
    /* Misc */ \
    X(node_empty) \
    X(declaration_list) \
    X(type_placeholder) \
    X(initalizer) \
    X(pointer) \
    X(dref_ptr) \
    X(array_access) \
    X(struct_access) \
    X(cast) \
    X(get_addr) \
    X(ptr_access) \
    X(declarator)

namespace Ast
{
    enum NodeType : uint16_t
    {
        #define X(name) name,
        #define OP(name) op_##name,
        #define ST(name) st_##name,

        NODE_TYPES

        #undef ST
        #undef OP
        #undef X
    };

        constexpr const char* nodeStr(Ast::NodeType type)
    {
        switch (type)
        {
            #define CASE(name) case NodeType::name: return #name;
            #define X(name) CASE(name)
            #define OP(name) CASE(op_##name)
            #define ST(name) CASE(st_##name)
            
            NODE_TYPES

            #undef ST
            #undef OP
            #undef X
            #undef CASE

        default:
            break;
        }
        return nullptr;
    }

    struct Node
    {
        Ast::NodeType type;
        Token token;
        Ast::Node* lChild;
        Ast::Node* rChild;
    };
}

#undef TOKEN_TYPES