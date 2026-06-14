module;
#include <print>
#include <string>
#include <unordered_map>
#include <format>
#include <variant>
#include <expected>

export module Interpreter;

import Stmt;
import Expr;
import Value;
import Token;
import Exceptions;
import Error;
import SourceUtils;

export class Interpreter {
private:
	std::string _source;

	//std::unordered_map<std::string, Variable> _variables;
	std::vector<std::unordered_map<std::string, Variable>> _scopes;

public:
	explicit Interpreter(std::string source) : _source(std::move(source)) {
		_scopes.emplace_back();
	}

	std::expected<void, Error> interpret(const Stmt* stmt);

private:
	void execute(const Stmt* stmt);
	Value evaluate(const Expr* expr);

	void begin_scope();
	void end_scope();

	Variable& lookup_variable(const std::string& name, Token token);
	bool type_matches(const Value& value, TokenType declared_type) const noexcept;
	bool is_truthy(const Value& value) const noexcept;
	double as_number(const Value& value) const;
	std::string as_string(const Value& value) const;
};

std::expected<void, Error> Interpreter::interpret(const Stmt* stmt)
{
	try {
		execute(stmt);

		return {};
	}
	catch (const RuntimeException& err) {
		return std::unexpected(
			Error{
				.code = err.code(),
				.message = err.what(),
				.line = err.token().line,
				.column = err.token().column,
				.source_line = get_line_from_source(_source, err.token().line),
			}
		);
	}
	catch (const std::runtime_error& err) {
		// other errors that might occur
		// temp -> remove this in future.
		return std::unexpected(
			Error{
				.code = ErrorCode::UNKNOWN,
				.message = err.what(),
				.line = 0,
				.column = 0,
				.source_line = "",
			}
		);
	}
}

void Interpreter::execute(const Stmt* stmt) {
	if (auto print_stmt = dynamic_cast<const PrintStmt*>(stmt)) {
		Value value = evaluate(print_stmt->expression.get());
		bool new_line = print_stmt->new_line;

		if (std::holds_alternative<double>(value)) {
			new_line 
				? std::println("{}", std::get<double>(value))
				: std::print("{}", std::get<double>(value));
		}
		else if (std::holds_alternative<std::string>(value)) {
			new_line 
				? std::println("{}", std::get<std::string>(value))
				: std::print("{}", std::get<std::string>(value));
		}
		else if (std::holds_alternative<bool>(value)) {
			new_line 
				? std::println("{}", std::get<bool>(value))
				: std::print("{}", std::get<bool>(value));
		}
		else {
			throw RuntimeException(
				ErrorCode::UNKNOWN,
				"Unknown type.",
				print_stmt->keyword
			);
		}

		return;
	}

	if (auto var = dynamic_cast<const VariableDeclarationStmt*>(stmt)) {
		auto& current_scope = _scopes.back();

		auto it = current_scope.find(var->name.lexeme);

		// ensure variable isnt already been defined.
		if (it != current_scope.end()) {
			throw RuntimeException(
				ErrorCode::ALREADY_DEFINED_VARIABLE,
				std::format(
					"Variable already defined '{}'.",
					var->name.lexeme
				),
				var->name
			);
		}

		Value value = evaluate(var->expression.get());

		// ensure type matches
		if (!type_matches(value, var->declared_type.type)) {
			throw RuntimeException(
				ErrorCode::TYPE_MISMATCH,
				std::format(
					"Expression returned incorrect data type. Expected '{}' return type.",
					var->declared_type.lexeme
				),
				var->name
			);
		}

		// creates the variable
		current_scope[var->name.lexeme] =
		{
			.declared_type = var->declared_type.type,
			.value = value,
		};

		return;
	}

	if (auto expression_stmt = dynamic_cast<const ExpressionStmt*>(stmt)) {
		evaluate(expression_stmt->expression.get());

		// evaluates and does nothing
		return;
	}

	if (auto block = dynamic_cast<const BlockStmt*>(stmt)) {
		begin_scope();

		try {
			for (const auto& statement : block->statements) {
				execute(statement.get());
			} 
		}
		catch (...) {
			end_scope();
			throw;
		}

		end_scope();

		return;
	}

	if (auto if_stmt = dynamic_cast<const IfStmt*>(stmt)) {
		auto condition_result = evaluate(if_stmt->condition.get());

		if (is_truthy(condition_result)) {
			execute(if_stmt->then_branch.get());
		}
		else if (if_stmt->else_branch.has_value()) {
			execute(if_stmt->else_branch.value().get());
		}
		else {
			throw RuntimeException(
				ErrorCode::IS_NOT_TRUTHY,
				"If condition expression doesn't evaluate to a boolean.",
				if_stmt->keyword
			);
		}

		return;
	}

	// Todo: come back to
	throw std::runtime_error{ "Unknown statement type." };

}

Value Interpreter::evaluate(const Expr* expr)
{
	if (auto literal = dynamic_cast<const LiteralExpr*>(expr)) {
		switch (literal->value.type) {
		case TokenType::NumberLiteral:
			return std::stod(literal->value.lexeme);
		case TokenType::StringLiteral:
			return literal->value.lexeme;
		case TokenType::True:
			return true;
		case TokenType::False:
			return false;

		default:
			throw RuntimeException(
				ErrorCode::UNKNOWN,
				"Unknown literal type.",
				literal->value
			);
		}
	}

	if (auto logical = dynamic_cast<const LogicalExpr*>(expr)) {
		Value left = evaluate(logical->left.get());

		if (logical->op.type == TokenType::Or) {
			if (is_truthy(left)) return left;
		}
		else {
			if (!is_truthy(left)) return left;
		}

		return evaluate(logical->right.get());
	}

	if (auto binary = dynamic_cast<const BinaryExpr*>(expr)) {
		Value left = evaluate(binary->left.get());
		Value right = evaluate(binary->right.get());

		try {
			switch (binary->op.type) {
			// Equality operators
			case TokenType::EqualEqual:
				return left == right;
			case TokenType::BangEqual:
				return left != right;

				// Comparison operators - only work for numbers, throw error otherwise
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
				// Numbers like numbers. Concatination for strings. Else throw error.
				if (std::holds_alternative<double>(left) &&
					std::holds_alternative<double>(right)) {
					return std::get<double>(left) + std::get<double>(right);
				}
				else if (std::holds_alternative<std::string>(left) &&
					std::holds_alternative<std::string>(right)) {
					return std::get<std::string>(left) + std::get<std::string>(right);
				}

				throw RuntimeException(
					ErrorCode::INVALID_OPERANDS,
					"Invalid operands for '+'. Both operands must be either numbers or strings.",
					binary->op
				);
			}
			case TokenType::Minus:
				return as_number(left) - as_number(right);
			case TokenType::Multiply:
				// todo: allow string * number for repeating strings ?
				return as_number(left) * as_number(right);
			case TokenType::Divide:
				if (as_number(left) == 0 || as_number(right) == 0) {
					// TODO: pass left or right depending on which ones 0
					throw RuntimeException(
						ErrorCode::DIVISION_BY_0,
						"Division by 0 is forbidden.",
						binary->op
					);
				}
				return as_number(left) / as_number(right);

			default:
				throw RuntimeException(
					ErrorCode::UNKNOWN,
					"Unkown binary operator.",
					binary->op
				);
			}

		}
		catch (const std::logic_error& err) {
			// thrown from as_number().

			// re-throw with custom error class
			throw RuntimeException(
				ErrorCode::TYPE_MISMATCH,
				err.what(),
				binary->op
			);
		}
	}

	if (auto unary = dynamic_cast<const UnaryExpr*>(expr)) {
		Value value = evaluate(unary->expr.get());

		switch (unary->op.type) {
		case TokenType::Minus: {
			// Numbers only.
			if (!std::holds_alternative<double>(value)) {
				throw RuntimeException(
					ErrorCode::INVALID_OPERAND,
					"Invalid operand for '-'. Operand must be a number.",
					unary->op
				);
			}

			return -std::get<double>(value);
		}
		case TokenType::Bang:
			// have a look at
			// currently C like (!0 = true = 1, !10 = false = 0)
			return !is_truthy(value);

		default:
			throw RuntimeException(
				ErrorCode::UNKNOWN,
				"Unkown unary operator.",
				unary->op
			);
		}
	}

	if (auto grouping = dynamic_cast<const GroupingExpr*>(expr)) {
		return evaluate(grouping->expr.get());
	}

	if (auto variable = dynamic_cast<const VariableExpr*>(expr)) {
		return lookup_variable(
			variable->name.lexeme,
			variable->name
		).value;
	}

	if (auto assignment = dynamic_cast<const AssignmentExpr*>(expr)) {
		Value value = evaluate(assignment->value.get());

		auto& variable = lookup_variable(
			assignment->name.lexeme,
			assignment->name
		);

		if (!type_matches(value, variable.declared_type)) {
			throw RuntimeException(
				ErrorCode::TYPE_MISMATCH,
				std::format(
					"Expression returned mismatched data type. Expected '{}' return type.",
					variable.get_type_name()
				),
				assignment->name
			);
		}

		variable.value = value;

		return value;
	}

	// Todo: come back to
	throw std::runtime_error{ "Unknown expression type." };
}

void Interpreter::begin_scope()
{
	_scopes.emplace_back();
}

void Interpreter::end_scope()
{
	_scopes.pop_back();
}

Variable& Interpreter::lookup_variable(const std::string& name, Token token)
{
	for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
		if (auto var = it->find(name); var != it->end()) {
			return var->second;
		}
	}

	throw RuntimeException(
		ErrorCode::UNDEFINED_VARIABLE,
		std::format("Undefined variable '{}'.", name),
		token	
	);
}

bool Interpreter::type_matches(const Value& value, TokenType declared_type) const noexcept
{
	 switch (declared_type)
		{
		case TokenType::Number:
			return std::holds_alternative<double>(value);

		case TokenType::String:
			return std::holds_alternative<std::string>(value);

		case TokenType::Bool:
			return std::holds_alternative<bool>(value);

		default:
			return false;
		}
}

bool Interpreter::is_truthy(const Value& value) const noexcept
{
	if (std::holds_alternative<bool>(value)) {
		return std::get<bool>(value);
	}

	// 0 = false, else true
	if (std::holds_alternative<double>(value)) {
		return std::get<double>(value) != 0;
	}

	// empty string = false, else true
	if (std::holds_alternative<std::string>(value)) {
		return !std::get<std::string>(value).empty();
	}

	return false;
}

double Interpreter::as_number(const Value& value) const
{
	if (!std::holds_alternative<double>(value)) {
		throw std::logic_error{ "Expected a number." };
	}

	return std::get<double>(value);
}

std::string Interpreter::as_string(const Value& value) const
{
	if (!std::holds_alternative<std::string>(value)) {
		throw std::logic_error{ "Expected a string." };
	}

	return std::get<std::string>(value);
}

