module;
#include <string>
#include <vector>
#include <algorithm>
#include <expected>

export module Lexer;

import Error;

/**
* Lexer involves taking our source into tokens.
*/

enum class TokenType {
	Number,

	Plus,
	Minus,
	Multiply,
	Divide,

	Eof
};

export struct Token {
	TokenType type;
	std::string lexeme;
};

export class Lexer {
private:
	std::string _source;
	size_t _current{ 0 };
	size_t _line{ 1 };
	size_t _column{ 0 };
	size_t _max;

public:
	Lexer(const std::string& source) : _source{ source }, _max{ source.size() } {}

	std::expected<std::vector<Token>, Error> scan_tokens();

	size_t lines() { return _line; }
	size_t column() { return _column; }
private:
	bool is_at_end() const;
	char advance();
	void consume(const std::string& t);

	bool is_digit(const char value);
	//bool is_numeric(const std::string& value) { return std::all_of(value.begin(), value.end(), is_digit); }
	bool is_whitespace(const char c);

	std::string get_current_line() const;
};



