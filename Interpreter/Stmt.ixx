module;
#include <memory>
#include <vector>

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

	PrintStmt(Token keyword, std::unique_ptr<Expr> expression)
		: keyword(std::move(keyword)), expression(std::move(expression))
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