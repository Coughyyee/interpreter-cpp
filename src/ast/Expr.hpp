#pragma once

#include <memory>
#include <vector>

#include "types/Token.hpp"

/* Expressions */

struct Expr
{
    virtual ~Expr() = default;
};

struct LiteralExpr : Expr
{
    Token value;

    LiteralExpr(Token value) : value(std::move(value)) {}
};

struct BinaryExpr : Expr
{
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right))
    {
    }
};

struct VariableExpr : Expr
{
    Token name;

    VariableExpr(Token name) : name(std::move(name)) {}
};

struct LogicalExpr : Expr
{
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    LogicalExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right))
    {
    }
};

struct UnaryExpr : Expr
{
    Token op;
    std::unique_ptr<Expr> expr;

    UnaryExpr(Token op, std::unique_ptr<Expr> expr) : op(std::move(op)), expr(std::move(expr)) {}
};

struct GroupingExpr : Expr
{
    std::unique_ptr<Expr> expr;

    GroupingExpr(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
};

struct AssignmentExpr : Expr
{
    Token name;
    std::unique_ptr<Expr> value;

    AssignmentExpr(Token name, std::unique_ptr<Expr> value) : name(std::move(name)), value(std::move(value)) {}
};

struct TypeOfExpr : Expr
{
    Token token;
    std::unique_ptr<Expr> expr;

    TypeOfExpr(Token token, std::unique_ptr<Expr> expr) : token(std::move(token)), expr(std::move(expr)) {}
};

struct ArrayExpr : Expr
{
    Token token;
    std::vector<std::unique_ptr<Expr>> elements;

    ArrayExpr(Token token, std::vector<std::unique_ptr<Expr>> elements)
        : token(std::move(token)), elements(std::move(elements))
    {
    }
};

struct IndexExpr : Expr
{
    Token token;
    std::unique_ptr<Expr> target;
    std::unique_ptr<Expr> index;

    IndexExpr(Token token, std::unique_ptr<Expr> target, std::unique_ptr<Expr> index)
        : token(std::move(token)), target(std::move(target)), index(std::move(index))
    {
    }
};

struct CallExpr : Expr
{
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;

    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments))
    {
    }
};
