#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "ast/Expr.hpp"
#include "types/Token.hpp"

/* Statements */

struct Stmt
{
    virtual ~Stmt() = default;
};

struct ExpressionStmt : Stmt
{
    Token keyword;
    std::unique_ptr<Expr> expression;

    ExpressionStmt(Token keyword, std::unique_ptr<Expr> expression)
        : keyword(std::move(keyword)), expression(std::move(expression))
    {
    }
};

struct PrintStmt : Stmt
{
    Token keyword;
    std::unique_ptr<Expr> expression;
    bool new_line; // true -> appends new line char to output.

    PrintStmt(Token keyword, std::unique_ptr<Expr> expression, bool new_line)
        : keyword(std::move(keyword)), expression(std::move(expression)), new_line(new_line)
    {
    }
};

struct VariableDeclarationStmt : Stmt
{
    Token keyword;
    Token name;
    Token declared_type;
    std::unique_ptr<Expr> expression;

    VariableDeclarationStmt(Token keyword, Token name, Token declared_type, std::unique_ptr<Expr> expression)
        : keyword(std::move(keyword)), name(std::move(name)), declared_type(std::move(declared_type)),
          expression(std::move(expression))
    {
    }
};

struct BlockStmt : Stmt
{
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}
};

struct LoopStmt : Stmt
{
    Token keyword;
    // if true then condition else infinite loop
    std::optional<std::unique_ptr<Expr>> condition;
    std::unique_ptr<Stmt> block;

    LoopStmt(Token keyword, std::optional<std::unique_ptr<Expr>> condition, std::unique_ptr<Stmt> block)
        : keyword(std::move(keyword)), condition(std::move(condition)), block(std::move(block))
    {
    }
};

struct IfStmt : Stmt
{
    Token keyword;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::optional<std::unique_ptr<Stmt>> else_branch;

    IfStmt(Token keyword, std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch,
           std::optional<std::unique_ptr<Stmt>> else_branch)
        : keyword(std::move(keyword)), condition(std::move(condition)), then_branch(std::move(then_branch)),
          else_branch(std::move(else_branch))
    {
    }
};

struct Parameter
{
    Token name;
    Token type;
};

struct FunctionDeclarationStmt : Stmt
{
    Token keyword;
    Token name;
    std::vector<Parameter> parameters;
    Token return_type;
    std::unique_ptr<Stmt> block;
    FunctionDeclarationStmt(Token keyword, Token name, std::vector<Parameter> parameters, Token return_type,
                            std::unique_ptr<Stmt> block)
        : keyword(std::move(keyword)), name(std::move(name)), parameters(std::move(parameters)),
          return_type(std::move(return_type)), block(std::move(block))
    {
    }
};

struct ReturnStmt : Stmt
{
    Token keyword;
    std::unique_ptr<Expr> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value) : keyword(std::move(keyword)), value(std::move(value)) {}
};
