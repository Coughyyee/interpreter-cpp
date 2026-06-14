module;
#include <memory>
#include <vector>
#include <optional>

export module Stmt;

import Expr;
import Token;

/* Statements */

export struct Stmt {
	virtual	~Stmt() = default;
};

export struct ExpressionStmt : Stmt {
	Token keyword;
	std::unique_ptr<Expr> expression;

	ExpressionStmt(Token keyword, std::unique_ptr<Expr> expression)
		: keyword(std::move(keyword)), expression(std::move(expression))
	{
	}
};

export struct PrintStmt : Stmt {
	Token keyword;
	std::unique_ptr<Expr> expression;
	bool new_line; // true -> appends new line char to output.

	PrintStmt(Token keyword, std::unique_ptr<Expr> expression, bool new_line)
		: keyword(std::move(keyword)), expression(std::move(expression)), new_line(new_line)
	{
	}
};

export struct VariableDeclarationStmt : Stmt {
	Token keyword;
	Token name;
	Token declared_type;
	std::unique_ptr<Expr> expression;

	VariableDeclarationStmt(Token keyword, Token name, Token declared_type, std::unique_ptr<Expr> expression)
		: keyword(std::move(keyword)), name(std::move(name)), declared_type(std::move(declared_type)), expression(std::move(expression))
	{
	}
};

export struct BlockStmt : Stmt {
	std::vector<std::unique_ptr<Stmt>> statements;

	BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
		: statements(std::move(statements))
	{
	}
};

export struct LoopStmt : Stmt {
	// if true then condition else infinite loop
	std::optional<std::unique_ptr<Expr>> condition;

	LoopStmt(std::optional<std::unique_ptr<Expr>> condition)
		: condition(std::move(condition))
	{
	}
};

export struct IfStmt : Stmt {
	Token keyword;
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> then_branch;
	std::optional<std::unique_ptr<Stmt>> else_branch;

	IfStmt(Token keyword, std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::optional<std::unique_ptr<Stmt>> else_branch)
		: keyword(std::move(keyword)), condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch))
	{
	}
};