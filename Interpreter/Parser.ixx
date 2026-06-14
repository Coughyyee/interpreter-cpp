module;
#include <vector>
#include <memory>
#include <string>
#include <expected>
#include <string_view>
#include <format>
#include <optional>

export module Parser;

import Token;
import Expr;
import Exceptions;
import Error;
import Stmt;
import SourceUtils;

export class Parser {
private:
	std::string _source;
	std::vector<Token> _tokens;
	size_t _current{ 0 };
	size_t _max;

public:
	Parser(std::string source, std::vector<Token>&& tokens)
		: _source(std::move(source)), _tokens(std::move(tokens)), _max(_tokens.size())
	{
	}
	
	std::expected<std::vector<std::unique_ptr<Stmt>>, Error> parse();

	std::unique_ptr<Stmt> statement();
	std::unique_ptr<Stmt> loop_statement();
	std::unique_ptr<Stmt> if_statement();
	std::unique_ptr<Stmt> variable_declaration_statement();
	std::unique_ptr<Stmt> print_statement();
	std::unique_ptr<Stmt> expression_statement();
	std::unique_ptr<Stmt> block_statement();
	std::unique_ptr<Expr> expression();
	std::unique_ptr<Expr> assignment();
	std::unique_ptr<Expr> or_expression();
	std::unique_ptr<Expr> and_expression();
	std::unique_ptr<Expr> equality();
	std::unique_ptr<Expr> comparison();
	std::unique_ptr<Expr> term();
	std::unique_ptr<Expr> factor();
	std::unique_ptr<Expr> unary();
	std::unique_ptr<Expr> primary();

private:
	bool is_at_end() const noexcept;
	Token& peek();
	const Token& peek() const;
	Token& previous();
	const Token& previous() const;
	Token advance();
	bool match(TokenType type);
	bool match(std::initializer_list<TokenType> types);
	bool check(TokenType type) const;
	Token consume(TokenType type, ErrorCode code, const std::string& message);
	Token consume(std::initializer_list<TokenType> types, ErrorCode code, const std::string& message);
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
				.source_line = get_line_from_source(_source, peek().line),
			}
			);
	}
}

std::unique_ptr<Stmt> Parser::statement()
{
	if (match(TokenType::Var)) {
		return variable_declaration_statement();
	}

	if (match(TokenType::LeftBrace)) {
		return block_statement();
	}

	if (match(TokenType::Out) || match(TokenType::Outln)) {
		return print_statement();
	}

	if (match(TokenType::Loop)) {
		return loop_statement();
	}

	if (match(TokenType::If)) {
		return if_statement();
	}

	return expression_statement();
}

std::unique_ptr<Stmt> Parser::loop_statement()
{
	Token keyword = previous(); // 'loop'
	
	// start of block
	if (peek().type == TokenType::LeftBrace) {

	}
	else {
		// expecting a expression condition that returns a bool type
		auto expression = equality();
	}

	// todo: implement 
	return std::make_unique<Stmt>();
}

std::unique_ptr<Stmt> Parser::if_statement()
{
	auto keyword = previous(); // get 'if'

	consume(
		TokenType::LeftParen,
		ErrorCode::EXPECTED,
		"Expected '(' after 'if'."
	);

	auto condition = expression();

	consume(
		TokenType::RightParen,
		ErrorCode::EXPECTED,
		"Expected ')' after if condition."
	);

	auto thenBranch = statement();
	std::optional<std::unique_ptr<Stmt>> elseBranch = std::nullopt;

	if (match(TokenType::Else)) {
		elseBranch = statement();
	}

	return std::make_unique<IfStmt>(
		std::move(keyword),
		std::move(condition), 
		std::move(thenBranch), 
		std::move(elseBranch)
	);
}

std::unique_ptr<Stmt> Parser::variable_declaration_statement()
{
	// var x -> int = 10;

	Token keyword = previous();

	Token name = consume(TokenType::Identifier, ErrorCode::EXPECTED, "Expected variable name.");
	
	consume(TokenType::Arrow, ErrorCode::EXPECTED, "Expected '->' after variable name.");

	Token declared_type = consume(
		{
			TokenType::Bool,
			TokenType::Number,
			TokenType::String,
		}, 
		ErrorCode::EXPECTED, 
		"Expected valid type name."
	);

	consume(TokenType::Equal, ErrorCode::EXPECTED, "Expected '=' after type.");

	auto expr = expression();

	consume(TokenType::Semicolon, ErrorCode::EXPECTED, "Expected ';' after expression.");

	return std::make_unique<VariableDeclarationStmt>(keyword, name, declared_type, std::move(expr));
}

std::unique_ptr<Stmt> Parser::print_statement() {
	Token keyword = previous(); 

	bool new_line = false;
	if (keyword.type == TokenType::Outln) {
		new_line = true;
	}

	auto value = expression();

	consume(
		TokenType::Semicolon,
		ErrorCode::EXPECTED,
		"Expected ';' after value."
	);

	return std::make_unique<PrintStmt>(keyword, std::move(value), new_line);
}

std::unique_ptr<Stmt> Parser::expression_statement() {
	Token start = peek();

	auto expr = expression();

	consume(
		TokenType::Semicolon,
		ErrorCode::EXPECTED,
		"Expected ';' after value."
	);

	return std::make_unique<ExpressionStmt>(start, std::move(expr));
}

std::unique_ptr<Stmt> Parser::block_statement()
{
	std::vector<std::unique_ptr<Stmt>> statements;

	while (
		!check(TokenType::RightBrace) &&
		!is_at_end()
		) {
		statements.push_back(statement());
	}

	consume(
		TokenType::RightBrace,
		ErrorCode::EXPECTED,
		"Expected '}' after block."
	);

	return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::expression()
{
	return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
	auto expr = or_expression();

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

std::unique_ptr<Expr> Parser::or_expression()
{
	auto expr = and_expression();

	while (match(TokenType::Or)) {
		auto op = previous();
		auto right = and_expression();
		expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::and_expression()
{
	auto expr = equality();

	while (match(TokenType::And)) {
		auto op = previous();
		auto right = equality();
		expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
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
	if (match(TokenType::Identifier)) {
		return std::make_unique<VariableExpr>(previous());
	}

	if (match({
		TokenType::NumberLiteral,
		TokenType::StringLiteral,
		TokenType::True,
		TokenType::False
	})) {
		return std::make_unique<LiteralExpr>(previous());
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

bool Parser::is_at_end() const noexcept {
	return _tokens.at(_current).type == TokenType::Eof;
}

/// <summary>
/// Retrieve current token at current position. 
/// </summary>
Token& Parser::peek() {
	return _tokens.at(_current);
}

/// <summary>
/// Retrieve a const current token at current position. 
/// </summary>
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

/// <summary>
/// Check if current token matches parameter. If true, calls advance() and returns true.
/// </summary>
bool Parser::match(TokenType type) {
	if (!check(type))
	{
		return false;
	}

	advance();
	return true;
}

bool Parser::match(std::initializer_list<TokenType> types)
{
	for (auto type : types) {
		if (check(type)) {
			advance();
			return true;
		}
	}

	return false;
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

Token Parser::consume(std::initializer_list<TokenType> types, ErrorCode code, const std::string& message)
{
	for (auto type : types) {
		if (check(type)) {
			return advance();
		}
	}

	throw ParserException(code, message);
}

