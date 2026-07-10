#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "ast/Expr.hpp"
#include "ast/Types.hpp"
#include "types/Token.hpp"

/* Statements */

/**
 * @brief Parent class for all statements. Each statement must implement a Token. Token should be a keyword or anything
 * that identifies the statement for error handling.
 */
struct Stmt
{
    Token token;
    explicit Stmt(Token token) : token(std::move(token)) {}

    virtual ~Stmt() = default;
};

/**
 * @brief Expression
 */
struct ExpressionStmt : Stmt
{
    std::unique_ptr<Expr> expression;

    ExpressionStmt(Token keyword, std::unique_ptr<Expr> expression) : Stmt(keyword), expression(std::move(expression))
    {
    }
};

/**
 * @brief Print statement, manages out / outln
 */
struct PrintStmt : Stmt
{
    std::unique_ptr<Expr> expression;
    bool new_line; // true -> appends new line char to output.

    PrintStmt(Token keyword, std::unique_ptr<Expr> expression, bool new_line)
        : Stmt(keyword), expression(std::move(expression)), new_line(new_line)
    {
    }
};

/**
 * @brief Manages variable declarations `var x -> <type> = <expr>;`
 */
struct VariableDeclarationStmt : Stmt
{
    Token name;
    Token declared_type;
    std::unique_ptr<Expr> expression;

    VariableDeclarationStmt(Token keyword, Token name, Token declared_type, std::unique_ptr<Expr> expression)
        : Stmt(keyword), name(std::move(name)), declared_type(std::move(declared_type)),
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

    BlockStmt(Token token, std::vector<std::unique_ptr<Stmt>> statements)
        : Stmt(token), statements(std::move(statements))
    {
    }
};

/**
 * @brief Manages loops `loop {..}` or `loop <condition> {...}`
 */
struct LoopStmt : Stmt
{
    // if true then condition else infinite loop
    std::optional<std::unique_ptr<Expr>> condition;
    std::unique_ptr<Stmt> block;

    LoopStmt(Token keyword, std::optional<std::unique_ptr<Expr>> condition, std::unique_ptr<Stmt> block)
        : Stmt(keyword), condition(std::move(condition)), block(std::move(block))
    {
    }
};

/**
 * @brief Manages if conditions `if (<condition>) {...}` and optional `else {...}`
 */
struct IfStmt : Stmt
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::optional<std::unique_ptr<Stmt>> else_branch;

    IfStmt(Token keyword, std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch,
           std::optional<std::unique_ptr<Stmt>> else_branch)
        : Stmt(keyword), condition(std::move(condition)), then_branch(std::move(then_branch)),
          else_branch(std::move(else_branch))
    {
    }
};

/* Functions */
/**
 * @brief Manages function declarations `func x(<params...>) -> <return-type> {...}`
 *
 * @param
 * @return
 * @throws
 */
struct FunctionDeclarationStmt : Stmt
{
    Token name;
    std::vector<FunctionParameter> parameters;
    Token return_type;
    std::unique_ptr<BlockStmt> block; // expects a blockstmt

    FunctionDeclarationStmt(Token keyword, Token name, std::vector<FunctionParameter> parameters, Token return_type,
                            std::unique_ptr<BlockStmt> block)
        : Stmt(keyword), name(std::move(name)), parameters(std::move(parameters)), return_type(std::move(return_type)),
          block(std::move(block))
    {
    }
};

/**
 * @brief Manages return expressions in functions.
 */
struct ReturnStmt : Stmt
{
    std::unique_ptr<Expr> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value) : Stmt(keyword), value(std::move(value)) {}
};

/**
 * @brief Manages object structures
 */
struct ObjectDeclarationStmt : Stmt
{
    Token name;
    std::vector<ObjectProperty> properties;

    ObjectDeclarationStmt(Token keyword, Token name, std::vector<ObjectProperty> properties)
        : Stmt(keyword), name(std::move(name)), properties(std::move(properties))
    {
    }
};
