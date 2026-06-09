module;
#include <vector>
#include <memory>
#include <string>
#include <expected>
#include <print>

export module Parser;

import Token;
import Expr;
import Error;
import Stmt;

export class Parser {
private:
	const std::string& _source;
	std::vector<Token> _tokens;
	size_t _current{ 0 };
	size_t _max;

public:
	Parser(const std::string& source, std::vector<Token>&& tokens)
		: _source(source), _tokens(std::move(tokens)), _max(_tokens.size())
	{
	}
	
	std::expected<std::vector<std::unique_ptr<Stmt>>, Error> parse();

	std::unique_ptr<Stmt> statement();
	std::unique_ptr<Stmt> print_statement();
	std::unique_ptr<Stmt> expression_statement();
	std::unique_ptr<Expr> expression();
	std::unique_ptr<Expr> term();
	std::unique_ptr<Expr> factor();
	std::unique_ptr<Expr> unary();
	std::unique_ptr<Expr> primary();

private:
	bool is_at_end() const;
	Token& peek();
	const Token& peek() const;
	Token& previous();
	const Token& previous() const;
	Token advance();
	bool match(TokenType type);
	bool check(TokenType type) const;
	Token consume(TokenType type, ErrorCode code, const std::string& message);
	std::string get_current_line() const;
};
