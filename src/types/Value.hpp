#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "ast/Stmt.hpp"
#include "types/Token.hpp"

template <typename T>
concept ArrayElementType = std::same_as<T, double> || std::same_as<T, bool> || std::same_as<T, std::string>;

struct ArrayValue;
using Value = std::variant<double, bool, std::string, std::shared_ptr<ArrayValue>>;
struct ArrayValue
{
    TokenType element_type;
    std::vector<Value> elements;
};

struct FunctionValue
{
    const std::vector<Parameter> parameters;
    const Token return_type;
    const BlockStmt* body;

    FunctionValue(std::vector<Parameter> params, Token ret_type, const BlockStmt* b)
        : parameters(std::move(params)), return_type(std::move(ret_type)), body(b)
    {
    }
};

struct Variable
{
    TokenType declared_type;
    Value value;

    [[nodiscard]]
    constexpr std::string_view get_type_name() const noexcept
    {
        using enum TokenType;

        switch (declared_type)
        {
        case Number:
            return "number";
        case Bool:
            return "bool";
        case String:
            return "string";
        case NumberArray:
            return "number[]";
        case BoolArray:
            return "bool[]";
        case StringArray:
            return "string[]";
        case Void:
            return "void";
        default:
            return "invalid";
        }
    }
};
