#pragma once

#include "ast/Expr.hpp"
#include "types/Token.hpp"
#include <memory>

/**
 * @brief For defining function parameters
 * @param name parameter name
 * @param type parameter type
 */
struct FunctionParameter
{
    Token name;
    Token type;
};

/**
 * @brief For defining a property inside of an object structure.
 * @param name property name
 * @param declared_type property type
 * @param value expression to be evaluted to return the value for the property.
 */
struct RecordProperty
{
    Token name;
    Token declared_type;
    std::unique_ptr<Expr> value;
};
