module;
#include <variant>
#include <string>
export module Value;

import Token;

// C++ types used within the interpreter.
export using Value = std::variant<double, bool, std::string>;

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
		default:
			return "invalid";
		}
	}
};
