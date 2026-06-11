module;
#include <memory>

export module Stmt;

import Expr;
import Token;

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

export struct VariableDeclarationStmt : Stmt {
	Token name;
	Token declared_type;
	std::unique_ptr<Expr> expression;


	VariableDeclarationStmt(Token name, Token declared_type, std::unique_ptr<Expr> expression)
		: name(std::move(name)), declared_type(std::move(declared_type)), expression(std::move(expression))
	{
	}
};