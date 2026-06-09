module;
#include <string>
#include <vector>
#include <algorithm>
#include <expected>

export module Lexer;

import Error;
import Token;

/**
* Lexer involves taking our source into tokens.
*/
export class Lexer {
private:
	const std::string& _source;
	size_t _current{ 0 };
	size_t _line{ 1 };
	size_t _column{ 0 };
	size_t _max;

public:
	Lexer(const std::string& source) : _source{ source }, _max{ source.size() } {}

	std::expected<std::vector<Token>, Error> scan_tokens();

	// helper functions
	size_t lines() { return _line; }
	size_t column() { return _column; }
private:
	Token number();
	Token identifier();

	char peek() const;
	char peek_next() const;
	char previous() const;
	char advance();

	bool is_alpha(char c);
	bool is_alphanumeric(char c);
	bool is_at_end() const;
	bool is_digit(const char value);
	//bool is_numeric(const std::string& value) { return std::all_of(value.begin(), value.end(), is_digit); }
	bool is_whitespace(const char c);

	std::string get_current_line() const;
};



