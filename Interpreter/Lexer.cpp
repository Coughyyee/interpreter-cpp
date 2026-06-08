module;
#include <string>
#include <vector>
#include <ranges>
#include <stdexcept>
#include <format>
#include <expected>
#include <print>

module Lexer;

std::expected<std::vector<Token>, Error> Lexer::scan_tokens() {
	// split on space
	// try to compare and turn into a token
	// store tokens in vector
	// return vector

	std::vector<Token> tokens{};

	while (!is_at_end()) {
		char c = advance();

		// omit white space
		if (is_whitespace(c)) {
			continue;
		}

		// tokens
		if (c == '+') {
			tokens.emplace_back(Token { TokenType::Plus, "+"});
			continue;
		}
		else if (c == '-') {
			tokens.emplace_back(Token { TokenType::Minus, "-"});
			continue;
		}
		else if (c == '*') {
			tokens.emplace_back(Token { TokenType::Multiply, "*"});
			continue;
		}
		else if (c == '/') {
			tokens.emplace_back(Token { TokenType::Divide, "/"});
			continue;
		}
		else {
			// check if numerical
			if (is_digit(c)) {
				std::string number{c};
				c = advance(); // consume first character

				// loop over numbers until whitespace
				while (!is_whitespace(c)) {
					// check if digit or a decimal point for floating numbers
					if (is_digit(c) || c == '.') {
						number.push_back(c);

						if (is_at_end()) 
							break;

						c = advance();
					}
					else {
						// unexpected character
						return std::unexpected(
							Error{
								.code = ErrorCode::BASIC,
								.message = "Invalid character within number value.",
								.line = _line,
								.column = _column,
								.source_line = get_current_line(),
							}
						);
					}
				}
				
				// append number token to array
				tokens.emplace_back(Token{ TokenType::Number, number });
			}
		}

		// EOF 
		if (c == '\0') {
			break;
		}
	}

	// EOF character at the end
	tokens.emplace_back(Token{ TokenType::Eof, ""});

	return tokens;
}

bool Lexer::is_at_end() const {
	// current == max -> final char
	return _current == _max;
}

/// <summary>
/// Returns current char in source. After returning char: _current++.
/// </summary>
/// <returns>Current char in source.</returns>
char Lexer::advance()
{
	if (is_at_end()) return '\0';

	// increment column by one for logging
	// P.s: _column on init = 0 so _column++ first time sets it to 1 on first char.
	_column++;

	// return character and increment current by one
	return _source.at(_current++);
}

void Lexer::consume(const std::string& t)
{
	// if source at _current:_current+t.size() is t
	// get substr
	std::string sub = _source.substr(_current, t.size());

	if (sub.compare(t) == 0) {
		// match
		_current += t.size();
	}
	else {
		throw std::runtime_error{ "Expected '" + t + "' token not found." };
	}
}

bool Lexer::is_digit(const char value)
{
	return std::isdigit(value); 
}

bool Lexer::is_whitespace(const char c)
{
	// if newline then change log tracking values
	if (c == '\n') {
		_line++;
		_column = 0;
	}

	return std::isspace(c); 
}

std::string Lexer::get_current_line() const
{
	size_t start = _current;

	while (start > 0 && _source[start - 1] != '\n') {
		--start;
	}

	size_t end = _current;

	while (end < _source.size() && _source[end] != '\n') {
		++end;
	}

	return _source.substr(start, end - start);
}
