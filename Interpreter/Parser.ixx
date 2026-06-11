module;
#include <vector>
#include <memory>
#include <string>
#include <expected>
#include <string_view>
#include <format>

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
	std::unique_ptr<Expr> assignment();
	std::unique_ptr<Expr> equality();
	std::unique_ptr<Expr> comparison();
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


std::expected<std::vector<std::unique_ptr<Stmt>>, Error> Parser::parse()
{
	try {
		std::vector<std::unique_ptr<Stmt>> statements;

		while (!is_at_end()) {
			statements.push_back(statement());
		}

		return statements;
	}
	catch (const ParserException& err) {
		return std::unexpected(
			Error{
				.code = err.code(),
				.message = err.what(),
				.line = peek().line,
				.column = peek().column,
				.source_line = get_current_line(),
			}
			);
	}
}

std::unique_ptr<Stmt> Parser::statement()
{
	if (match(TokenType::Print)) {
		return print_statement();
	}

	return expression_statement();
}

std::unique_ptr<Stmt> Parser::print_statement() {
	auto value = expression();

	consume(
		TokenType::Semicolon,
		ErrorCode::EXPECTED,
		"Expected ';' after value."
	);

	return std::make_unique<PrintStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::expression_statement() {
	auto expr = expression();

	consume(
		TokenType::Semicolon,
		ErrorCode::EXPECTED,
		"Expected ';' after value."
	);

	return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Expr> Parser::expression()
{
	return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
	auto expr = equality();

	if (match(TokenType::Equal)) {
		auto value = assignment();

		if (auto variable = dynamic_cast<VariableExpr*>(expr.get())) {
			return std::make_unique<AssignmentExpr>(
				variable->name,
				std::move(value)
			);
		}

		throw ParserException(ErrorCode::INVALID_ASSIGNMENT, "Invalid assignment target.");
	}

	return expr;
}

std::unique_ptr<Expr> Parser::equality()
{
	auto expr = comparison();

	while (
		match(TokenType::EqualEqual) ||
		match(TokenType::BangEqual)
		) {
		Token op = previous();

		auto right = comparison();

		expr = std::make_unique<BinaryExpr>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}

std::unique_ptr<Expr> Parser::comparison()
{
	auto expr = term();

	while (
		match(TokenType::MoreThan) ||
		match(TokenType::MoreThanEqual) || 
		match(TokenType::LessThan) ||
		match(TokenType::LessThanEqual) 
		) {
		Token op = previous();

		auto right = term();

		expr = std::make_unique<BinaryExpr>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}


std::unique_ptr<Expr> Parser::term()
{
	auto expr = factor();

	// + -
	while (
		match(TokenType::Plus) ||
		match(TokenType::Minus)
		) {
		Token op = previous();

		auto right = factor();

		expr = std::make_unique<BinaryExpr>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}

std::unique_ptr<Expr> Parser::factor()
{
	auto expr = unary();

	// * /
	while (
		match(TokenType::Multiply) ||
		match(TokenType::Divide)
		) {
		Token op = previous();

		auto right = unary();

		expr = std::make_unique<BinaryExpr>(
			std::move(expr),
			op,
			std::move(right)
		);
	}

	return expr;
}

std::unique_ptr<Expr> Parser::unary()
{
	// - !
	if (match(TokenType::Minus) || match(TokenType::Bang)) {
		Token op = previous();

		auto right = unary();

		return std::make_unique<UnaryExpr>(
			op,
			std::move(right)
		);
	}

	return primary();
}

std::unique_ptr<Expr> Parser::primary()
{
	// Identifiers
	if (match(TokenType::Identifier)) {
		return std::make_unique<VariableExpr>(previous());
	}

	// Just a number
	if (match(TokenType::Number)) {
		return std::make_unique<LiteralExpr>(
			previous()
		);
	}

	// String
	if (match(TokenType::String)) {
		return std::make_unique<LiteralExpr>(
			previous()
		);
	}

	// Group start (...)
	if (match(TokenType::LeftParen)) {
		auto expr = expression();

		consume(
			TokenType::RightParen,
			ErrorCode::EXPECTED,
			"Expected ')' after expression."
		);

		return std::make_unique<GroupingExpr>(
			std::move(expr)
		);
	}

	// Throw?
	throw ParserException(ErrorCode::UNKNOWN, "Parser Error.");
}

bool Parser::is_at_end() const {
	return _tokens.at(_current).type == TokenType::Eof;
}

Token& Parser::peek() {
	return _tokens.at(_current);
}

const Token& Parser::peek() const {
	return _tokens.at(_current);
}

Token& Parser::previous() {
	return _tokens.at(_current - 1);
}

const Token& Parser::previous() const
{
	return _tokens.at(_current - 1);
}

Token Parser::advance() {
	if (!is_at_end()) {
		++_current;
	}
	return previous();
}

bool Parser::match(TokenType type) {
	if (!check(type))
	{
		return false;
	}

	advance();
	return true;
}

bool Parser::check(TokenType type) const {
	return peek().type == type;
}

Token Parser::consume(TokenType type, ErrorCode code, const std::string& message) {
	if (check(type)) {
		return advance();
	}

	throw ParserException(
		code, 
		std::format(
			"{} Instead found '{}'.",
			message,
			peek().lexeme
		)
	);
}

std::string Parser::get_current_line() const
{
	size_t targetLine = peek().line;

	size_t currentLine = 1;
	size_t start = 0;

	for (size_t i = 0; i < _source.size(); ++i)
	{
		if (currentLine == targetLine)
		{
			start = i;
			break;
		}

		if (_source[i] == '\n')
		{
			++currentLine;
		}
	}

	size_t end = start;

	while (
		end < _source.size() &&
		_source[end] != '\n'
		)
	{
		++end;
	}

	return _source.substr(start, end - start);
}
