module;
#include <variant>
#include <string>
#include <vector>
#include <memory>
export module Value;

import Token;

export struct ArrayValue;
export using Value = std::variant<double, bool, std::string, std::shared_ptr<ArrayValue>>;
export struct ArrayValue {
	std::vector<Value> elements;
};

export template<typename T>
concept ArrayElementType =
	std::same_as<T, double> ||
	std::same_as<T, bool> ||
	std::same_as<T, std::string>;

export struct Variable {
	TokenType declared_type;
	Value value;

	std::string_view get_type_name() const {
		switch (declared_type) {
		case TokenType::Number:
			return "number";
		case TokenType::Bool:
			return "bool";
		case TokenType::String:
			return "string";
		case TokenType::NumberArray:
			return "number[]";
		case TokenType::BoolArray:
			return "bool[]";
		case TokenType::StringArray:
			return "string[]";
		default:
			return "invalid";
		}
	}
};
