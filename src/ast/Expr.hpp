#pragma once

#include <memory>
#include <vector>

#include "types/Token.hpp"

/* Expressions */

/**
 * @brief Parent class for all Exprs. Each expression must implement a Token. Token should be a keyword or anything that
 * identifies the statement for error handling.
 */
struct Expr
{
    Token token;
    explicit Expr(Token token) : token(std::move(token)) {}

    virtual ~Expr() = default;
};

struct LiteralExpr : Expr
{
    LiteralExpr(Token value) : Expr(value) {}
};

struct BinaryExpr : Expr
{
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : Expr(op), left(std::move(left)), right(std::move(right))
    {
    }
};

struct VariableExpr : Expr
{
    VariableExpr(Token name) : Expr(name) {}
};

struct LogicalExpr : Expr
{
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    LogicalExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : Expr(op), left(std::move(left)), right(std::move(right))
    {
    }
};

struct UnaryExpr : Expr
{
    std::unique_ptr<Expr> expr;

    UnaryExpr(Token op, std::unique_ptr<Expr> expr) : Expr(op), expr(std::move(expr)) {}
};

struct GroupingExpr : Expr
{
    std::unique_ptr<Expr> expr;

    GroupingExpr(Token token, std::unique_ptr<Expr> expr) : Expr(token), expr(std::move(expr)) {}
};

struct AssignmentExpr : Expr
{
    std::unique_ptr<Expr> value;

    AssignmentExpr(Token name, std::unique_ptr<Expr> value) : Expr(name), value(std::move(value)) {}
};

struct TypeOfExpr : Expr
{
    std::unique_ptr<Expr> expr;

    TypeOfExpr(Token token, std::unique_ptr<Expr> expr) : Expr(token), expr(std::move(expr)) {}
};

struct ArrayExpr : Expr
{
    std::vector<std::unique_ptr<Expr>> elements;

    ArrayExpr(Token token, std::vector<std::unique_ptr<Expr>> elements) : Expr(token), elements(std::move(elements)) {}
};

struct IndexExpr : Expr
{
    std::unique_ptr<Expr> target;
    std::unique_ptr<Expr> index;

    IndexExpr(Token token, std::unique_ptr<Expr> target, std::unique_ptr<Expr> index)
        : Expr(token), target(std::move(target)), index(std::move(index))
    {
    }
};

struct CallExpr : Expr
{
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> arguments;

    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : Expr(paren), callee(std::move(callee)), arguments(std::move(arguments))
    {
    }
};
