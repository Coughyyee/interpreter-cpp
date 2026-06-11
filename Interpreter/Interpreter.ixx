module;
#include <print>
#include <string>
#include <unordered_map>
#include <format>
#include <variant>

export module Interpreter;

// TODO: implement customer error handling, implement types.

import Stmt;
import Expr;

// C++ types used within the interpreter.
using Value = std::variant<double, bool, std::string>;

export class Interpreter {
private:
	std::unordered_map<std::string, Value> _variables;

public:
	void interpret(const Stmt* stmt);

private:
	Value evaluate(const Expr* expr);
	double as_number(const Value& value) const;
	std::string as_string(const Value& value) const;
	bool is_truthy(const Value& value) const;
};

void Interpreter::interpret(const Stmt* stmt)
{
	if (auto print_stmt = dynamic_cast<const PrintStmt*>(stmt)) {
		Value value = evaluate(print_stmt->expression.get());

		if (std::holds_alternative<double>(value)) {
			std::print("{}", std::get<double>(value));
		}
		else if (std::holds_alternative<std::string>(value)) {
			std::print("{}", std::get<std::string>(value));
		}
		else if (std::holds_alternative<bool>(value)) {
			std::print("{}", std::get<bool>(value) ? "true" : "false");
		}
		else {
			throw std::runtime_error{ "Unknown value type." };
		}

		return;
	}

	if (auto expression_stmt = dynamic_cast<const ExpressionStmt*>(stmt)) {
		evaluate(expression_stmt->expression.get());

		// evaluates and does nothing
		return;
	}

	throw std::runtime_error{ "Unknown statement type." };
}

Value Interpreter::evaluate(const Expr* expr)
{
	if (auto literal = dynamic_cast<const LiteralExpr*>(expr)) {
		switch (literal->value.type) {
		case TokenType::Number:
			return std::stod(literal->value.lexeme);
			
		case TokenType::String:
			return literal->value.lexeme	;

		default:
			throw std::runtime_error{ "Unknown literal type." };
		}
	}

	if (auto binary = dynamic_cast<const BinaryExpr*>(expr)) {
		Value left = evaluate(binary->left.get());
		Value right = evaluate(binary->right.get());

		switch (binary->op.type) {
		// Equality operators
		case TokenType::EqualEqual:
			return left == right;
		case TokenType::BangEqual:
			return left != right;

		// Comparison operators
		case TokenType::MoreThan:
			return as_number(left) > as_number(right);
		case TokenType::MoreThanEqual:
			return as_number(left) >= as_number(right);
		case TokenType::LessThan:
			return as_number(left) < as_number(right);
		case TokenType::LessThanEqual:
			return as_number(left) <= as_number(right);

		// Mathematical operators
		case TokenType::Plus: {
			// number + number
			if (std::holds_alternative<double>(left) &&
				std::holds_alternative<double>(right)) {
				return std::get<double>(left) + std::get<double>(right);
			}
			else if (std::holds_alternative<std::string>(left) &&
				std::holds_alternative<std::string>(right)) {
				return std::get<std::string>(left) + std::get<std::string>(right);
			}

			throw std::runtime_error{ "Invalid operands for '+'. Both operands must be either numbers or strings." };
		}
		case TokenType::Minus:
			return as_number(left) - as_number(right);
		case TokenType::Multiply:
			// todo: allow string * number for repeating strings ?
			return as_number(left) * as_number(right);
		case TokenType::Divide:
			return as_number(left) / as_number(right);

		default:
			throw std::runtime_error{ "Unknown binary operator." };
		}
	}

	if (auto unary = dynamic_cast<const UnaryExpr*>(expr)) {
		Value value = evaluate(unary->expr.get());

		switch (unary->op.type) {
		case TokenType::Minus: {
			if (!std::holds_alternative<double>(value)) {
				throw std::runtime_error{ "Invalid operand for '-'. Operand must be a number." };
			}

			return -std::get<double>(value);
		}
		case TokenType::Bang:
			// have a look at
			// currently C like (!0 = true = 1, !10 = false = 0)
			return !is_truthy(value);

		default:
			throw std::runtime_error{ "Unknown unary operator." };
		}
	}

	if (auto grouping = dynamic_cast<const GroupingExpr*>(expr)) {
		return evaluate(grouping->expr.get());
	}

	if (auto variable = dynamic_cast<const VariableExpr*>(expr)) {
		// look for variable in map
		auto it = _variables.find(variable->name.lexeme);

		// doesnt exist
		if (it == _variables.end()) {
			throw std::runtime_error{ std::format("Undefined variable '{}'.", variable->name.lexeme) };
		}
		
		// return stored value
		return it->second;
	}

	if (auto assignment = dynamic_cast<const AssignmentExpr*>(expr)) {
		Value value = evaluate(assignment->value.get());

		_variables[assignment->name.lexeme] = value;

		return value;
	}

	throw std::runtime_error{ "Unknown expression type." };
}

double Interpreter::as_number(const Value& value) const
{
	if (!std::holds_alternative<double>(value)) {
		throw std::runtime_error{ "Expected a number." };
	}

	return std::get<double>(value);
}

std::string Interpreter::as_string(const Value& value) const
{
	if (!std::holds_alternative<std::string>(value)) {
		throw std::runtime_error{ "Expected a string." };
	}

	return std::get<std::string>(value);
}

bool Interpreter::is_truthy(const Value& value) const
{
	if (std::holds_alternative<bool>(value)) {
		return std::get<bool>(value);
	}

	// 0 = false, else true
	if (std::holds_alternative<double>(value)) {
		return std::get<double>(value) != 0;
	}

	if (std::holds_alternative<std::string>(value)) {
		return !std::get<std::string>(value).empty();
	}

	return false;
}
