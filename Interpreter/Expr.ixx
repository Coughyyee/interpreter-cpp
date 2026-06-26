module;
#include <memory>
#include <vector>

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

export struct LogicalExpr : Expr {
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	LogicalExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right))
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

export struct TypeOfExpr : Expr {
	std::unique_ptr<Expr> expr;

	TypeOfExpr(std::unique_ptr<Expr> expr)
		: expr(std::move(expr))
	{
	}
};

export struct ArrayExpr : Expr {
	std::vector<std::unique_ptr<Expr>> elements;

	ArrayExpr(std::vector<std::unique_ptr<Expr>> elements)
		: elements(std::move(elements))
	{
	}
};

export struct IndexExpr : Expr {
	Token token;
	std::unique_ptr<Expr> target;
	std::unique_ptr<Expr> index;

	IndexExpr(Token token, std::unique_ptr<Expr> target, std::unique_ptr<Expr> index)
		: token(std::move(token)), target(std::move(target)), index(std::move(index))
	{
	}
};

export struct CallExpr : Expr {
	std::unique_ptr<Expr> callee;
	Token paren;
	std::vector<std::unique_ptr<Expr>> arguments;

	CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
		: callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments))
	{
	}
};

