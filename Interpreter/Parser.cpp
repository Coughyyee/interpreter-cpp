module;
#include <string_view>
#include <expected>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>

module Parser;

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
	return term();
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
	// Just a number
	if (match(TokenType::Number)) {
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
    if (is_at_end())
    {
        return false;
    }

    return peek().type == type;
}

Token Parser::consume(TokenType type, ErrorCode code, const std::string& message) {
	if (check(type))
	{
		return advance();
	}

	throw ParserException(code, message);
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
