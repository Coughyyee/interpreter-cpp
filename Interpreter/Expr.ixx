module;
#include <memory>

export module Expr;

import Token;

/* Expressions */

export struct Expr {
	virtual ~Expr() = default;
};

export struct LiteralExpr : Expr {
	Token value;

	LiteralExpr(Token value)
		: value(std::move(value))
	{
	}
};

export struct BinaryExpr : Expr {
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	BinaryExpr(std::unique_ptr<Expr>left, Token op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right))
	{
	}
};

export struct VariableExpr : Expr {
	Token name;

	VariableExpr(Token name)
		: name(std::move(name))
	{
	}
};

export struct UnaryExpr : Expr {
	Token op;
	std::unique_ptr<Expr> expr;

	UnaryExpr(Token op, std::unique_ptr<Expr> expr)
		: op(std::move(op)), expr(std::move(expr))
	{
	}
};

export struct GroupingExpr : Expr {
	std::unique_ptr<Expr> expr;

	GroupingExpr(std::unique_ptr<Expr> expr)
		: expr(std::move(expr))
	{
	}
};

export struct AssignmentExpr : Expr {
	Token name;
	std::unique_ptr<Expr> value;

	AssignmentExpr(Token name, std::unique_ptr<Expr> value)
		: name(std::move(name)), value(std::move(value))
	{
	}
};

// Later: Function, class, enum etc.
