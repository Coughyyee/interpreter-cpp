#pragma once

#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/Expr.hpp"
#include "ast/Stmt.hpp"
#include "types/Error.hpp"
#include "types/Value.hpp"

class Interpreter
{
  private:
    const std::string& _source;

    // std::unordered_map<std::string, Variable> _variables;
    std::vector<std::unordered_map<std::string, Variable>> _scopes;

    std::unordered_map<std::string, std::shared_ptr<FunctionValue>> _functions;
    std::optional<Value> _return_value;

  public:
    /**
     * @brief ctor - creates initial global scope in _scopes
     *
     * @param source reference to source file
     */
    explicit Interpreter(const std::string& source) : _source(source)
    {
        _scopes.emplace_back();
    }

    std::expected<void, Error> interpret(const Stmt* stmt);

  private:
    /**
     * @brief Executes statements
     * @param stmt Passed in stmt to get executed
     */
    void execute(const Stmt* stmt);

    /**
     * @brief Evaluate an expression and return a Value
     * @param expr Expression to be evaluated
     * @return Value returned from expression evaluation
     * @throws RuntimeException - Provide proper run time errors
     * @throws std::runtime_error - typically thrown when error is UNKNOWN (aka an internal error) - could change in
     * future.
     */
    Value evaluate(const Expr* expr);

#pragma region execution
    void execute_print(const PrintStmt* stmt);
    void execute_variable_declaration(const VariableDeclarationStmt* stmt);
    void execute_object_declaration(const ObjectDeclarationStmt* stmt);
    void execute_expression(const ExpressionStmt* stmt);
    void execute_block(const BlockStmt* stmt);
    void execute_if(const IfStmt* stmt);
    void execute_loop(const LoopStmt* stmt);
    void execute_function_declaration(const FunctionDeclarationStmt* stmt);
    void execute_return(const ReturnStmt* stmt);
#pragma endregion

#pragma region evaluation
    Value evaluate_literal(const LiteralExpr* expr);
    Value evaluate_logic(const LogicalExpr* expr);
    Value evaluate_binary(const BinaryExpr* expr);
    Value evaluate_unary(const UnaryExpr* expr);
    Value evaluate_type_of(const TypeOfExpr* expr);
    Value evaluate_group(const GroupingExpr* expr);
    Value evaluate_variable(const VariableExpr* expr);
    Value evaluate_assignment(const AssignmentExpr* expr);
    Value evaluate_array(const ArrayExpr* expr);
    Value evaluate_index(const IndexExpr* expr);
    Value evaluate_call(const CallExpr* expr);
#pragma endregion

    void begin_scope();
    void end_scope();

    Value call_function(const std::shared_ptr<FunctionValue>& function, const std::vector<Value>& arguments,
                        Token call_token);

    Variable& lookup_variable(const std::string& name, Token token);

    /**
     * @throws logic error if value not double.
     */
    [[nodiscard]] static double as_number(const Value& value);
    /**
     * @throws logic error if value not string.
     */
    [[nodiscard]] static std::string as_string(const Value& value);

    [[nodiscard]] static bool type_matches(const Value& value, TokenType declared_type) noexcept;
    template <ArrayElementType T> [[nodiscard]] static bool type_matches_array(const Value& value);
    [[nodiscard]] static bool is_truthy(const Value& value) noexcept;
};
