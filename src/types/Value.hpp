#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "ast/Stmt.hpp"
#include "ast/Types.hpp"
#include "types/Token.hpp"

// TODO: Append object value?
template <typename T>
concept ArrayElementType = std::same_as<T, double> || std::same_as<T, bool> || std::same_as<T, std::string>;

struct ArrayValue;
struct ObjectValue;
using Value = std::variant<double, bool, std::string, std::shared_ptr<ArrayValue>, std::shared_ptr<ObjectValue>>;
struct ArrayValue
{
    TokenType element_type;
    std::vector<Value> elements;
};

struct FunctionValue
{
    const std::vector<FunctionParameter> parameters;
    const Token return_type;
    const BlockStmt* body;

    FunctionValue(std::vector<FunctionParameter> params, Token ret_type, const BlockStmt* b)
        : parameters(std::move(params)), return_type(std::move(ret_type)), body(b)
    {
    }
};

struct Variable
{
    TokenType declared_type;
    Value value;

    [[nodiscard]]
    std::string get_value_to_string() const noexcept
    {
        if (std::holds_alternative<double>(value))
        {
            return std::to_string(std::get<double>(value));
        }
        else if (std::holds_alternative<std::string>(value))
        {
            return std::get<std::string>(value);
        }
        else if (std::holds_alternative<bool>(value))
        {
            return std::get<bool>(value) ? "true" : "false";
        }
        // Array printing
        else if (std::holds_alternative<std::shared_ptr<ArrayValue>>(value))
        {
            auto array = std::get<std::shared_ptr<ArrayValue>>(value);
            std::stringstream ss;
            ss << '[';

            for (size_t i = 0; i < array->elements.size(); ++i)
            {
                const auto& element = array->elements[i];

                if (std::holds_alternative<double>(element))
                {
                    ss << std::get<double>(element);
                }
                else if (std::holds_alternative<std::string>(element))
                {
                    ss << std::get<std::string>(element);
                }
                else if (std::holds_alternative<bool>(element))
                {
                    ss << (std::get<bool>(element) ? "true" : "false");
                }

                // append comma to list
                if (i + 1 < array->elements.size())
                {
                    ss << ", ";
                }
            }

            ss << ']';
            return ss.str();
        }

        return "";
    }

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
        case Object: // ? Allowed?
            return "object";
        case Void:
            return "void";
        default:
            return "invalid";
        }
    }
};

struct ObjectValue
{
    std::string object_name;
    /**
     * @brief std::string - property name, Variable - prop data
     */
    std::unordered_map<std::string, Variable> properties;
};
