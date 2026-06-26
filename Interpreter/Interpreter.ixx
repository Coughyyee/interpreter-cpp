module;
#include <print>
#include <string>
#include <unordered_map>
#include <format>
#include <variant>
#include <expected>
#include <optional>

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

	std::unordered_map<std::string, std::shared_ptr<FunctionValue>> _functions;
	std::optional<Value> _return_value;

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

	Value call_function(const std::shared_ptr<FunctionValue>& function, const std::vector<Value>& arguments, Token call_token);

	Variable& lookup_variable(const std::string& name, Token token);
	bool type_matches(const Value& value, TokenType declared_type) const noexcept;
	template <ArrayElementType T> bool type_matches_array(const Value& value) const;
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
		// Array printing
		else if (std::holds_alternative<std::shared_ptr<ArrayValue>>(value)) {
			auto array = std::get<std::shared_ptr<ArrayValue>>(value);
			std::print("[");

			for (size_t i = 0; i < array->elements.size(); ++i) {
				const auto& element = array->elements[i];

				if (std::holds_alternative<double>(element)) {
					std::print("{}", std::get<double>(element));
				}
				else if (std::holds_alternative<std::string>(element)) {
					std::print("{}", std::get<std::string>(element));
				}
				else if (std::holds_alternative<bool>(element)) {
					std::print("{}", std::get<bool>(element));
				}

				if (i + 1 < array->elements.size()) {
					std::print(", ");
				}
			}

			std::print("]");

			// if outln used
			if (new_line) {
				std::print("\n");
			}
		}
		// Error
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

		// if branch
		if (is_truthy(condition_result)) {
			execute(if_stmt->then_branch.get());
		}
		// else branch
		else if (if_stmt->else_branch.has_value()) {
			execute(if_stmt->else_branch.value().get());
		}
	
		return;
	}

	if (auto loop_stmt = dynamic_cast<const LoopStmt*>(stmt)) {
		if (loop_stmt->condition.has_value()) {
			while (true) {
				auto condition_result = evaluate(loop_stmt->condition.value().get());

				// re-evaluate condition - break when condition is false
				if (!is_truthy(condition_result)) {
					break;
				}

				execute(loop_stmt->block.get());
			}
		}
		else {
			// infinite loop
			while (true) {
				execute(loop_stmt->block.get());
			}
		}

		return;
	}

	if (auto func_decl = dynamic_cast<const FunctionDeclarationStmt*>(stmt)) {
		auto function = std::make_shared<FunctionValue>(
			func_decl->parameters,
			func_decl->return_type,
			std::unique_ptr<Stmt>(const_cast<Stmt*>(func_decl->block.get()))
		);

		// Release ownership from original stmt's unique_ptr
		const_cast<FunctionDeclarationStmt*>(func_decl)->block.release();

		_functions[func_decl->name.lexeme] = function;
		return;
	}

	if (auto return_stmt = dynamic_cast<const ReturnStmt*>(stmt)) {
		_return_value = evaluate(return_stmt->value.get());
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

	if (auto type_of = dynamic_cast<const TypeOfExpr*>(expr)) {
		Value value = evaluate(type_of->expr.get());

		if (std::holds_alternative<double>(value))
			return std::string("number");
		if (std::holds_alternative<bool>(value))
			return std::string("bool");
		if (std::holds_alternative<std::string>(value))
			return std::string("string");
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

	if (auto array = dynamic_cast<const ArrayExpr*>(expr)) {
		auto result = std::make_shared<ArrayValue>();

		for (const auto& element : array->elements) {
			result->elements.push_back(
				evaluate(element.get())
			);
		}

		return result;
	}

	if (auto index = dynamic_cast<const IndexExpr*>(expr)) {
		Value target = evaluate(index->target.get());
		if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(target)) {
			throw RuntimeException(
				ErrorCode::INVALID_OPERAND,
				"Invalid operand for index. Target must be an array.",
				index->token
			);
		}

		Value index_value = evaluate(index->index.get());
		if (!std::holds_alternative<double>(index_value)) {
			throw RuntimeException(
				ErrorCode::INVALID_OPERAND,
				"Invalid operand for index. Index must be a number.",
				index->token
			);
		}

		int index_value_int = static_cast<int>(std::get<double>(index_value)); // cast type to int for array indexing

		auto array = std::get<std::shared_ptr<ArrayValue>>(target);

		if (index_value_int < 0 || index_value_int >= array->elements.size()) {
			throw RuntimeException(
				ErrorCode::INDEX_OUT_OF_BOUNDS,
				std::format(
					"Index out of bounds. Index {} is not valid for array of size {}.",
					index_value_int,
					array->elements.size()
				),
				index->token
			);
		}

		return array->elements[index_value_int];
	}

	if (auto call = dynamic_cast<const CallExpr*>(expr)) {
		// Evaluate the callee - should be a variable pointing to a function
		if (auto variable = dynamic_cast<const VariableExpr*>(call->callee.get())) {
			auto it = _functions.find(variable->name.lexeme);
			if (it == _functions.end()) {
				throw RuntimeException(
					ErrorCode::UNDEFINED_VARIABLE,
					std::format("Undefined function '{}'.", variable->name.lexeme),
					call->paren
				);
			}

			// Evaluate all arguments
			std::vector<Value> arg_values;
			for (const auto& arg : call->arguments) {
				arg_values.push_back(evaluate(arg.get()));
			}

			// Call the function
			return call_function(it->second, arg_values, call->paren);
		}

		throw RuntimeException(
			ErrorCode::INVALID_OPERAND,
			"Can only call functions.",
			call->paren
		);
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

Value Interpreter::call_function(const std::shared_ptr<FunctionValue>& function, const std::vector<Value>& arguments, Token call_token)
{
	// Check argument count
	if (arguments.size() != function->parameters.size()) {
		throw RuntimeException(
			ErrorCode::INVALID_OPERAND,
			std::format(
				"Expected {} arguments but got {}.",
				function->parameters.size(),
				arguments.size()
			),
			call_token
		);
	}

	// Create new scope for function
	begin_scope();
	auto& func_scope = _scopes.back();

	// Bind parameters
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& param = function->parameters[i];
		const auto& arg_value = arguments[i];

		// Type check parameter
		if (!type_matches(arg_value, param.type.type)) {
			end_scope();
			throw RuntimeException(
				ErrorCode::TYPE_MISMATCH,
				std::format(
					"Parameter '{}' type mismatch. Expected '{}' but got different type.",
					param.name.lexeme,
					param.type.lexeme
				),
				call_token
			);
		}

		func_scope[param.name.lexeme] = Variable{
			param.type.type,
			arg_value
		};
	}

	// Execute function body
	_return_value = std::nullopt;
	try {
		execute(function->body.get());
	}
	catch (...) {
		end_scope();
		throw;
	}

	// Type check return value if not void
	if (function->return_type.type != TokenType::Void) {
		if (_return_value.has_value() && !type_matches(_return_value.value(), function->return_type.type)) {
			end_scope();
			throw RuntimeException(
				ErrorCode::TYPE_MISMATCH,
				std::format(
					"Function return type mismatch. Expected '{}' but got different type.",
					function->return_type.lexeme
				),
				call_token
			);
		}
	}

	Value result = _return_value.value_or(Value{});
	_return_value = std::nullopt;
	end_scope();

	return result;
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

	case TokenType::NumberArray: 
		return type_matches_array<double>(value);
	case TokenType::StringArray: 
		return type_matches_array<std::string>(value);
	case TokenType::BoolArray: 
		return type_matches_array<bool>(value);

	case TokenType::Void:
		// Void should never be used in type_matches for variable declarations
		// It's only for function return types, handled separately
		return false;

	default:
		return false;
	}
}

template <ArrayElementType T>
bool Interpreter::type_matches_array(const Value& value) const
{
	if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(value)) {
		return false;
	}

	auto array = std::get<std::shared_ptr<ArrayValue>>(value);
	for (const auto& element : array->elements) {
		if (!std::holds_alternative<T>(element)) {
			return false;
		}
	}

	return true;
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

