#pragma once

#include <variant>
#include <string>
#include <vector>
#include <memory>

#include "types/Token.hpp"
// #include "ast/Stmt.hpp" // TODO: for Parameter and STMT

template<typename T>
concept ArrayElementType =
	std::same_as<T, double> ||
	std::same_as<T, bool> ||
	std::same_as<T, std::string>;

struct ArrayValue;
using Value = std::variant<double, bool, std::string, std::shared_ptr<ArrayValue>>;
struct ArrayValue {
	std::vector<Value> elements;
};

struct FunctionValue {
	std::vector<Parameter> parameters;
	Token return_type;
	std::unique_ptr<Stmt> body;

	FunctionValue(std::vector<Parameter> params, Token ret_type, std::unique_ptr<Stmt> b)
		: parameters(std::move(params)), return_type(std::move(ret_type)), body(std::move(b))
	{
	}
};

struct Variable {
	TokenType declared_type;
	Value value;

	[[nodiscard]]
	constexpr std::string_view get_type_name() const noexcept {
	    using enum TokenType;

		switch (declared_type) {
		case Number: return "number";
		case Bool: return "bool";
		case String: return "string";
		case NumberArray: return "number[]";
		case BoolArray: return "bool[]";
		case StringArray: return "string[]";
		case Void: return "void";
		default: return "invalid";
		}
	}
};
