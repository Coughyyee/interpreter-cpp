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

/**
 * @brief Expression
 */
struct ExpressionStmt : Stmt
{
    Token keyword;
    std::unique_ptr<Expr> expression;

    ExpressionStmt(Token keyword, std::unique_ptr<Expr> expression)
        : keyword(std::move(keyword)), expression(std::move(expression))
    {
    }
};

/**
 * @brief Print statement, manages out / outln
 */
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

/**
 * @brief Manages variable declarations `var x -> <type> = <expr>;`
 */
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

/**
 * @brief Manages blocks
 */
struct BlockStmt : Stmt
{
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}
};

/**
 * @brief Manages loops `loop {..}` or `loop <condition> {...}`
 */
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

/**
 * @brief Manages if conditions `if (<condition>) {...}` and optional `else {...}`
 */
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

/* Functions */
struct Parameter
{
    Token name;
    Token type;
};

/**
 * @brief Manages function declarations `func x(<params...>) -> <return-type> {...}`
 *
 * @param
 * @return
 * @throws
 */
struct FunctionDeclarationStmt : Stmt
{
    Token keyword;
    Token name;
    std::vector<Parameter> parameters;
    Token return_type;
    std::unique_ptr<BlockStmt> block; // expects a blockstmt

    FunctionDeclarationStmt(Token keyword, Token name, std::vector<Parameter> parameters, Token return_type,
                            std::unique_ptr<BlockStmt> block)
        : keyword(std::move(keyword)), name(std::move(name)), parameters(std::move(parameters)),
          return_type(std::move(return_type)), block(std::move(block))
    {
    }
};

/**
 * @brief Manages return expressions in functions.
 */
struct ReturnStmt : Stmt
{
    Token keyword;
    std::unique_ptr<Expr> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value) : keyword(std::move(keyword)), value(std::move(value)) {}
};
