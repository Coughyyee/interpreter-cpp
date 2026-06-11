module;
#include <memory>

export module Stmt;

import Expr;

/* Statements */

export struct Stmt {
	virtual	~Stmt() = default;
};

export struct ExpressionStmt : Stmt {
	std::unique_ptr<Expr> expression;

	ExpressionStmt(std::unique_ptr<Expr> expression)
		: expression(std::move(expression))
	{
	}
};

export struct PrintStmt : Stmt {
	std::unique_ptr<Expr> expression;

	PrintStmt(std::unique_ptr<Expr> expression)
		: expression(std::move(expression))
	{
	}
};