#include "interpreter/Interpreter.hpp"

#include "types/Exceptions.hpp"
#include "types/Value.hpp"
#include "utils/SourceUtils.hpp"
#include <print>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

std::expected<void, Error> Interpreter::interpret(const Stmt* stmt)
{
    try
    {
        execute(stmt);

        return {};
    }
    catch (const RuntimeException& err)
    {
        return std::unexpected(Error{
            .code = err.code(),
            .message = err.what(),
            .line = err.token().line,
            .column = err.token().column,
            .source_line = get_line_from_source(_source, err.token().line),
        });
    }
    catch (const std::runtime_error& err)
    {
        // other errors that might occur
        // temp -> remove this in future.
        return std::unexpected(Error{
            .code = ErrorCode::UNKNOWN,
            .message = err.what(),
            .line = 0,
            .column = 0,
            .source_line = "",
        });
    }
}

void Interpreter::execute(const Stmt* stmt)
{
    if (auto print_stmt = dynamic_cast<const PrintStmt*>(stmt))
    {
        return execute_print(print_stmt);
    }

    if (auto var_stmt = dynamic_cast<const VariableDeclarationStmt*>(stmt))
    {
        return execute_variable_declaration(var_stmt);
    }

    if (auto expr_stmt = dynamic_cast<const ExpressionStmt*>(stmt))
    {
        return execute_expression(expr_stmt);
    }

    if (auto block_stmt = dynamic_cast<const BlockStmt*>(stmt))
    {
        return execute_block(block_stmt);
    }

    if (auto if_stmt = dynamic_cast<const IfStmt*>(stmt))
    {
        return execute_if(if_stmt);
    }

    if (auto loop_stmt = dynamic_cast<const LoopStmt*>(stmt))
    {
        return execute_loop(loop_stmt);
    }

    if (auto func_decl_stmt = dynamic_cast<const FunctionDeclarationStmt*>(stmt))
    {
        return execute_function_declaration(func_decl_stmt);
    }

    if (auto return_stmt = dynamic_cast<const ReturnStmt*>(stmt))
    {
        return execute_return(return_stmt);
    }

    if (auto obj_decl_stmt = dynamic_cast<const ObjectDeclarationStmt*>(stmt))
    {
        return execute_object_declaration(obj_decl_stmt);
    }

    // Todo: come back to
    throw std::runtime_error{"Unknown statement type."};
}

Value Interpreter::evaluate(const Expr* expr)
{
    if (auto literal_expr = dynamic_cast<const LiteralExpr*>(expr))
    {
        return evaluate_literal(literal_expr);
    }

    if (auto logical_expr = dynamic_cast<const LogicalExpr*>(expr))
    {
        return evaluate_logic(logical_expr);
    }

    if (auto binary_expr = dynamic_cast<const BinaryExpr*>(expr))
    {
        return evaluate_binary(binary_expr);
    }

    if (auto unary_expr = dynamic_cast<const UnaryExpr*>(expr))
    {
        return evaluate_unary(unary_expr);
    }

    if (auto type_of_expr = dynamic_cast<const TypeOfExpr*>(expr))
    {
        return evaluate_type_of(type_of_expr);
    }

    if (auto grouping_expr = dynamic_cast<const GroupingExpr*>(expr))
    {
        return evaluate_group(grouping_expr);
    }

    if (auto variable_expr = dynamic_cast<const VariableExpr*>(expr))
    {
        return evaluate_variable(variable_expr);
    }

    if (auto assignment_expr = dynamic_cast<const AssignmentExpr*>(expr))
    {
        return evaluate_assignment(assignment_expr);
    }

    if (auto array_expr = dynamic_cast<const ArrayExpr*>(expr))
    {
        return evaluate_array(array_expr);
    }

    if (auto index_expr = dynamic_cast<const IndexExpr*>(expr))
    {
        return evaluate_index(index_expr);
    }

    if (auto call_expr = dynamic_cast<const CallExpr*>(expr))
    {
        return evaluate_call(call_expr);
    }

    // Todo: come back to
    throw std::runtime_error{"Unknown expression type."};
}

#pragma region execution
void Interpreter::execute_print(const PrintStmt* stmt)
{
    Value value = evaluate(stmt->expression.get());
    bool new_line = stmt->new_line;

    // TODO:  extract value to struct with a get_value_to_string() function?
    // Or just extract code.
    if (std::holds_alternative<double>(value))
    {
        new_line ? std::println("{}", std::get<double>(value)) : std::print("{}", std::get<double>(value));
    }
    else if (std::holds_alternative<std::string>(value))
    {
        new_line ? std::println("{}", std::get<std::string>(value)) : std::print("{}", std::get<std::string>(value));
    }
    else if (std::holds_alternative<bool>(value))
    {
        new_line ? std::println("{}", std::get<bool>(value)) : std::print("{}", std::get<bool>(value));
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

            if (i + 1 < array->elements.size())
            {
                ss << ", ";
            }
        }
        ss << ']';

        // if outln used
        if (new_line)
        {
            ss << "\n";
        }

        std::print("{}", ss.str());
    }
    else if (std::holds_alternative<std::shared_ptr<ObjectValue>>(value))
    {
        auto object = std::get<std::shared_ptr<ObjectValue>>(value);

        // Foo -> { x = 10, y = "hi" }
        std::print("{} -> {{ ", object->object_name);

        std::size_t count = 0;
        for (const auto& [name, value] : object->properties)
        {
            if (std::holds_alternative<std::string>(value.value))
            {
                // surround string with quotes
                std::print("{} = \"{}\"", name, value.get_value_to_string());
            }
            else
            {
                std::print("{} = {}", name, value.get_value_to_string());
            }

            if (++count < object->properties.size())
            {
                std::print(", ");
            }
            else
            {
                std::print(" "); // single space after final prop - looks nicer
            }
        }
        std::print("}}");
        if (new_line)
        {
            std::print("\n");
        }
    }
    // Error
    else
    {
        throw RuntimeException(ErrorCode::UNKNOWN, "Unknown type.", stmt->token);
    }
}
void Interpreter::execute_object_declaration(const ObjectDeclarationStmt* stmt)
{
    auto& current_scope = _scopes.back();

    auto it = current_scope.find(stmt->name.lexeme);

    // ensure object isnt already been defined.
    if (it != current_scope.end())
    {
        throw RuntimeException(ErrorCode::ALREADY_DEFINED_VARIABLE,
                               std::format("Object already defined '{}'.", stmt->name.lexeme), stmt->name);
    }

    std::unordered_map<std::string, Variable> props{};

    // loop over properties
    for (auto& prop : stmt->properties)
    {
        Value value = evaluate(prop.value.get()); // evaluate prop expr

        // if return value of expression doesnt match declared type
        if (!type_matches(value, prop.declared_type.type))
        {
            /// Error cursor points to name of property

            // if array type return slightly different error message
            if (std::holds_alternative<std::shared_ptr<ArrayValue>>(value))
            {
                throw RuntimeException(
                    ErrorCode::TYPE_MISMATCH,
                    std::format(
                        "Expression returned incorrect data type(s). Expected array to contain values of '{}' type.",
                        prop.declared_type.lexeme),
                    prop.name);
            }

            // non-array error
            throw RuntimeException(
                ErrorCode::TYPE_MISMATCH,
                std::format("Expression returned incorrect data type. Expected '{}' type.", prop.declared_type.lexeme),
                prop.name);
        }

        // valid expr returned (type matches declared type)
        props[prop.name.lexeme] = {
            .declared_type = prop.declared_type.type,
            .value = value,
        };
    }

    // creates the object in scope
    current_scope[stmt->name.lexeme] = {
        .declared_type = TokenType::Object,
        .value = std::make_shared<ObjectValue>(stmt->name.lexeme, props),
    };
}
void Interpreter::execute_variable_declaration(const VariableDeclarationStmt* stmt)
{
    auto& current_scope = _scopes.back();

    auto it = current_scope.find(stmt->name.lexeme);

    // ensure variable isnt already been defined.
    if (it != current_scope.end())
    {
        throw RuntimeException(ErrorCode::ALREADY_DEFINED_VARIABLE,
                               std::format("Variable already defined '{}'.", stmt->name.lexeme), stmt->name);
    }

    Value value = evaluate(stmt->expression.get());

    // ensure type matches
    if (!type_matches(value, stmt->declared_type.type))
    {
        // if array type return slightly different error message
        if (std::holds_alternative<std::shared_ptr<ArrayValue>>(value))
        {
            throw RuntimeException(
                ErrorCode::TYPE_MISMATCH,
                std::format(
                    "Expression returned incorrect data type(s). Expected array to contain values of '{}' type.",
                    stmt->declared_type.lexeme),
                stmt->token);
        }

        // general error
        throw RuntimeException(
            ErrorCode::TYPE_MISMATCH,
            std::format("Expression returned incorrect data type. Expected '{}' type.", stmt->declared_type.lexeme),
            stmt->token);
    }

    // creates the variable
    current_scope[stmt->name.lexeme] = {
        .declared_type = stmt->declared_type.type,
        .value = value,
    };
}
void Interpreter::execute_expression(const ExpressionStmt* stmt)
{
    // evaluates and does nothing
    evaluate(stmt->expression.get());
}
void Interpreter::execute_block(const BlockStmt* stmt)
{
    begin_scope();

    try
    {
        for (const auto& statement : stmt->statements)
        {
            execute(statement.get());
        }
    }
    catch (...)
    {
        end_scope();
        throw;
    }

    end_scope();
}
void Interpreter::execute_if(const IfStmt* stmt)
{
    auto condition_result = evaluate(stmt->condition.get());

    // if branch
    if (is_truthy(condition_result))
    {
        execute(stmt->then_branch.get());
    }
    // else branch
    else if (stmt->else_branch.has_value())
    {
        execute(stmt->else_branch.value().get());
    }
}
void Interpreter::execute_loop(const LoopStmt* stmt)
{
    if (stmt->condition.has_value())
    {
        while (true)
        {
            auto condition_result = evaluate(stmt->condition.value().get());

            // re-evaluate condition - break when condition is false
            if (!is_truthy(condition_result))
            {
                break;
            }

            execute(stmt->block.get());
        }
    }
    else
    {
        // infinite loop
        while (true)
        {
            execute(stmt->block.get());
        }
    }
}
void Interpreter::execute_function_declaration(const FunctionDeclarationStmt* stmt)
{
    // Release ownership from original stmt's unique_ptr
    auto function = std::make_shared<FunctionValue>(stmt->parameters, stmt->return_type, stmt->block.get());

    _functions[stmt->name.lexeme] = function;
}
void Interpreter::execute_return(const ReturnStmt* stmt)
{
    _return_value = evaluate(stmt->value.get());
}
#pragma endregion

#pragma region evaluation
Value Interpreter::evaluate_literal(const LiteralExpr* expr)
{
    switch (expr->token.type)
    {
    case TokenType::NumberLiteral:
        return std::stod(expr->token.lexeme);
    case TokenType::StringLiteral:
        return expr->token.lexeme;
    case TokenType::True:
        return true;
    case TokenType::False:
        return false;

    default:
        throw RuntimeException(ErrorCode::UNKNOWN, "Unknown literal type.", expr->token);
    }
}
Value Interpreter::evaluate_logic(const LogicalExpr* expr)
{
    Value left = evaluate(expr->left.get());

    if (expr->token.type == TokenType::Or)
    {
        if (is_truthy(left))
            return left;
    }
    else
    {
        if (!is_truthy(left))
            return left;
    }

    return evaluate(expr->right.get());
}
Value Interpreter::evaluate_binary(const BinaryExpr* expr)
{
    Value left = evaluate(expr->left.get());
    Value right = evaluate(expr->right.get());

    try
    {
        switch (expr->token.type)
        {
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
        case TokenType::Plus:
        {
            // Numbers like numbers. Concatination for strings. Else throw error.
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                return std::get<double>(left) + std::get<double>(right);
            }
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
            {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }

            throw RuntimeException(ErrorCode::INVALID_OPERANDS,
                                   "Invalid operands for '+'. Both operands must be either numbers or strings.",
                                   expr->token);
        }
        case TokenType::Minus:
            return as_number(left) - as_number(right);
        case TokenType::Multiply:
            // todo: allow string * number for repeating strings ?
            return as_number(left) * as_number(right);
        case TokenType::Divide:
            if (as_number(left) == 0 || as_number(right) == 0)
            {
                // TODO: pass left or right depending on which ones 0
                throw RuntimeException(ErrorCode::DIVISION_BY_0, "Division by 0 is forbidden.", expr->token);
            }
            return as_number(left) / as_number(right);

        default:
            throw RuntimeException(ErrorCode::UNKNOWN, "Unkown binary operator.", expr->token);
        }
    }
    catch (const std::logic_error& err)
    {
        // thrown from as_number().

        // re-throw with custom error class
        throw RuntimeException(ErrorCode::TYPE_MISMATCH, err.what(), expr->token);
    }
}
Value Interpreter::evaluate_unary(const UnaryExpr* expr)
{
    Value value = evaluate(expr->expr.get());

    switch (expr->token.type)
    {
    case TokenType::Minus:
    {
        // Numbers only.
        if (!std::holds_alternative<double>(value))
        {
            throw RuntimeException(ErrorCode::INVALID_OPERAND, "Invalid operand for '-'. Operand must be a number.",
                                   expr->token);
        }

        return -std::get<double>(value);
    }
    case TokenType::Bang:
        // have a look at
        // currently C like (!0 = true = 1, !10 = false = 0)
        return !is_truthy(value);

    default:
        throw RuntimeException(ErrorCode::UNKNOWN, "Unkown unary operator.", expr->token);
    }
}
Value Interpreter::evaluate_type_of(const TypeOfExpr* expr)
{
    Value value = evaluate(expr->expr.get());

    if (std::holds_alternative<double>(value))
        return std::string("number");
    if (std::holds_alternative<bool>(value))
        return std::string("bool");
    if (std::holds_alternative<std::string>(value))
        return std::string("string");
    if (std::holds_alternative<std::shared_ptr<ArrayValue>>(value))
    {
        // array checker
        auto array = std::get<std::shared_ptr<ArrayValue>>(value);

        switch (array->element_type)
        {
        case TokenType::Number:
            return std::string("number[]");

        case TokenType::String:
            return std::string("string[]");

        case TokenType::Bool:
            return std::string("bool[]");

        default:
            return std::string("array");
        }
    }
    if (std::holds_alternative<std::shared_ptr<ObjectValue>>(value))
        return std::string("object");

    throw RuntimeException(ErrorCode::INVALID_TYPE, "Expression returns invalid type.", expr->token);
}
Value Interpreter::evaluate_group(const GroupingExpr* expr)
{
    return evaluate(expr->expr.get());
}
Value Interpreter::evaluate_variable(const VariableExpr* expr)
{
    return lookup_variable(expr->token.lexeme, expr->token).value;
}
Value Interpreter::evaluate_assignment(const AssignmentExpr* expr)
{
    Value value = evaluate(expr->value.get());

    auto& variable = lookup_variable(expr->token.lexeme, expr->token);

    if (!type_matches(value, variable.declared_type))
    {
        throw RuntimeException(ErrorCode::TYPE_MISMATCH,
                               std::format("Expression returned mismatched data type. Expected '{}' return type.",
                                           variable.get_type_name()),
                               expr->token);
    }

    variable.value = value;

    return value;
}
Value Interpreter::evaluate_array(const ArrayExpr* expr)
{
    auto result = std::make_shared<ArrayValue>();

    // Empty array
    if (expr->elements.empty())
    {
        result->element_type = TokenType::Void;
        return result;
    }

    // Evaluate first element
    Value first = evaluate(expr->elements[0].get());

    result->elements.push_back(first);

    // Determine runtime type
    if (std::holds_alternative<double>(first))
    {
        result->element_type = TokenType::Number;
    }
    else if (std::holds_alternative<std::string>(first))
    {
        result->element_type = TokenType::String;
    }
    else if (std::holds_alternative<bool>(first))
    {
        result->element_type = TokenType::Bool;
    }
    else
    {
        throw RuntimeException(ErrorCode::INVALID_TYPE, "Invalid array element type.", expr->token);
    }

    // Remaining elements
    for (size_t i = 1; i < expr->elements.size(); ++i)
    {
        Value value = evaluate(expr->elements[i].get());

        if (!type_matches(value, result->element_type))
        {
            throw RuntimeException(ErrorCode::TYPE_MISMATCH, "Array contains mixed element types.", expr->token);
        }

        result->elements.push_back(std::move(value));
    }

    return result;
}
Value Interpreter::evaluate_index(const IndexExpr* expr)
{
    Value target = evaluate(expr->target.get());
    if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(target))
    {
        throw RuntimeException(ErrorCode::INVALID_OPERAND, "Invalid operand for index. Target must be an array.",
                               expr->token);
    }

    Value index_value = evaluate(expr->index.get());
    if (!std::holds_alternative<double>(index_value))
    {
        throw RuntimeException(ErrorCode::INVALID_OPERAND, "Invalid operand for index. Index must be a number.",
                               expr->token);
    }

    int index_value_int = static_cast<int>(std::get<double>(index_value)); // cast type to int for array indexing

    auto array = std::get<std::shared_ptr<ArrayValue>>(target);

    if (index_value_int < 0 || index_value_int >= array->elements.size())
    {
        throw RuntimeException(ErrorCode::INDEX_OUT_OF_BOUNDS,
                               std::format("Index out of bounds. Index {} is not valid for array of size {}.",
                                           index_value_int, array->elements.size()),
                               expr->token);
    }

    return array->elements[index_value_int];
}
Value Interpreter::evaluate_call(const CallExpr* expr)
{
    // Evaluate the callee - should be a variable pointing to a function
    if (auto variable = dynamic_cast<const VariableExpr*>(expr->callee.get()))
    {
        auto it = _functions.find(variable->token.lexeme);
        if (it == _functions.end())
        {
            throw RuntimeException(ErrorCode::UNDEFINED_VARIABLE,
                                   std::format("Undefined function '{}'.", variable->token.lexeme), expr->token);
        }

        // Evaluate all arguments
        std::vector<Value> arg_values;
        for (const auto& arg : expr->arguments)
        {
            arg_values.push_back(evaluate(arg.get()));
        }

        // Call the function
        return call_function(it->second, arg_values, expr->token);
    }

    throw RuntimeException(ErrorCode::INVALID_OPERAND, "Can only call functions.", expr->token);
}
#pragma endregion

void Interpreter::begin_scope()
{
    _scopes.emplace_back();
}

void Interpreter::end_scope()
{
    _scopes.pop_back();
}

Value Interpreter::call_function(const std::shared_ptr<FunctionValue>& function, const std::vector<Value>& arguments,
                                 Token call_token)
{
    // Check argument count
    if (arguments.size() != function->parameters.size())
    {
        throw RuntimeException(
            ErrorCode::INVALID_OPERAND,
            std::format("Expected {} arguments but got {}.", function->parameters.size(), arguments.size()),
            call_token);
    }

    // Create new scope for function
    begin_scope();
    auto& func_scope = _scopes.back();

    // Bind parameters
    for (size_t i = 0; i < arguments.size(); ++i)
    {
        const auto& param = function->parameters[i];
        const auto& arg_value = arguments[i];

        // Type check parameter
        if (!type_matches(arg_value, param.type.type))
        {
            end_scope();
            throw RuntimeException(ErrorCode::TYPE_MISMATCH,
                                   std::format("Parameter '{}' type mismatch. Expected '{}' but got different type.",
                                               param.name.lexeme, param.type.lexeme),
                                   call_token);
        }

        func_scope[param.name.lexeme] = Variable{param.type.type, arg_value};
    }

    // Execute function body
    _return_value = std::nullopt;
    try
    {
        execute(function->body);
    }
    catch (...)
    {
        end_scope();
        throw;
    }

    // Type check return value if not void
    if (function->return_type.type != TokenType::Void)
    {
        if (_return_value.has_value() && !type_matches(_return_value.value(), function->return_type.type))
        {
            end_scope();
            throw RuntimeException(ErrorCode::TYPE_MISMATCH,
                                   std::format("Function return type mismatch. Expected '{}' but got different type.",
                                               function->return_type.lexeme),
                                   call_token);
        }
    }

    Value result = _return_value.value_or(Value{});
    _return_value = std::nullopt;
    end_scope();

    return result;
}

Variable& Interpreter::lookup_variable(const std::string& name, Token token)
{
    for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it)
    {
        if (auto var = it->find(name); var != it->end())
        {
            return var->second;
        }
    }

    throw RuntimeException(ErrorCode::UNDEFINED_VARIABLE, std::format("Undefined variable '{}'.", name), token);
}

double Interpreter::as_number(const Value& value)
{
    if (!std::holds_alternative<double>(value))
    {
        throw std::logic_error{"Expected a number."};
    }

    return std::get<double>(value);
}

std::string Interpreter::as_string(const Value& value)
{
    if (!std::holds_alternative<std::string>(value))
    {
        throw std::logic_error{"Expected a string."};
    }

    return std::get<std::string>(value);
}

bool Interpreter::type_matches(const Value& value, TokenType declared_type) noexcept
{
    using enum TokenType;

    switch (declared_type)
    {
    case Number:
        return std::holds_alternative<double>(value);
    case String:
        return std::holds_alternative<std::string>(value);
    case Bool:
        return std::holds_alternative<bool>(value);

    case NumberArray:
        return type_matches_array<double>(value);
    case StringArray:
        return type_matches_array<std::string>(value);
    case BoolArray:
        return type_matches_array<bool>(value);

    case Void:
        // Void should never be used in type_matches for variable declarations
        // It's only for function return types, handled separately
        return false;

    default:
        return false;
    }
}

template <ArrayElementType T> bool Interpreter::type_matches_array(const Value& value)
{
    if (!std::holds_alternative<std::shared_ptr<ArrayValue>>(value))
    {
        return false;
    }

    auto array = std::get<std::shared_ptr<ArrayValue>>(value);

    if constexpr (std::same_as<T, double>)
    {
        return array->element_type == TokenType::Number;
    }

    if constexpr (std::same_as<T, std::string>)
    {
        return array->element_type == TokenType::String;
    }

    if constexpr (std::same_as<T, bool>)
    {
        return array->element_type == TokenType::Bool;
    }

    return false;
}

bool Interpreter::is_truthy(const Value& value) noexcept
{
    if (std::holds_alternative<bool>(value))
    {
        return std::get<bool>(value);
    }

    // 0 = false, else true
    if (std::holds_alternative<double>(value))
    {
        return std::get<double>(value) != 0;
    }

    // empty string = false, else true
    if (std::holds_alternative<std::string>(value))
    {
        return !std::get<std::string>(value).empty();
    }

    return false;
}
