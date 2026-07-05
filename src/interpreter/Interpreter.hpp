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
    explicit Interpreter(const std::string& source) : _source(source)
    {
        _scopes.emplace_back();
    }

    std::expected<void, Error> interpret(const Stmt* stmt);

  private:
    void execute(const Stmt* stmt);
    Value evaluate(const Expr* expr);

    void begin_scope();
    void end_scope();

    Value call_function(const std::shared_ptr<FunctionValue>& function, const std::vector<Value>& arguments,
                        Token call_token);

    Variable& lookup_variable(const std::string& name, Token token);
    [[nodiscard]] bool type_matches(const Value& value, TokenType declared_type) const noexcept;
    template <ArrayElementType T> bool type_matches_array(const Value& value) const;
    [[nodiscard]] bool is_truthy(const Value& value) const noexcept;

    /**
     * @throws logic error if value not double.
     */
    [[nodiscard]] double as_number(const Value& value) const;
    /**
     * @throws logic error if value not string.
     */
    [[nodiscard]] std::string as_string(const Value& value) const;
};
