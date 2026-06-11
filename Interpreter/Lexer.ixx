module;
#include <string>
#include <vector>
#include <algorithm>
#include <expected>
#include <ranges>
#include <format>

export module Lexer;

import Error;
import Token;

/*
Lexer involves taking our source into tokens.
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

std::expected<std::vector<Token>, Error> Lexer::scan_tokens()
{
    std::vector<Token> tokens;

    while (!is_at_end()) {
        size_t line = _line;
        size_t column = _column + 1;

        char c = advance();

        if (is_whitespace(c))
            continue;

        switch (c) {
        case '!':
            if (peek() == '=') {
                advance();
				tokens.emplace_back(Token{TokenType::BangEqual, "!=", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::Bang, "!", line, column});
            break;
        case '=':
            if (peek() == '=') {
                advance();
				tokens.emplace_back(Token{TokenType::EqualEqual, "==", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::Equal, "=", line, column});
            break;
        case '>':
            if (peek() == '=') {
                advance();
				tokens.emplace_back(Token{TokenType::MoreThanEqual, ">=", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::MoreThan, ">", line, column});
            break;
        case '<':
            if (peek() == '=') {
                advance();
				tokens.emplace_back(Token{TokenType::LessThanEqual, "<=", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::LessThan, "<", line, column});
            break;
        case '+':
            tokens.emplace_back(Token{TokenType::Plus, "+", line, column});
            break;

        case '-':
            tokens.emplace_back(Token{TokenType::Minus, "-", line, column});
            break;

        case '*':
            tokens.emplace_back(Token{TokenType::Multiply, "*", line, column});
            break;

        case '/':
            tokens.emplace_back(Token{TokenType::Divide, "/", line, column});
            break;

        case '(':
            tokens.emplace_back(Token{TokenType::LeftParen, "(", line, column});
            break;

        case ')':
            tokens.emplace_back(Token{TokenType::RightParen, ")", line, column});
            break;

        case '{':
            tokens.emplace_back(Token{TokenType::LeftBrace, "{", line, column});
            break;

        case '}':
            tokens.emplace_back(Token{TokenType::RightBrace, "}", line, column});
            break;

        case ';':
            tokens.emplace_back(Token{TokenType::Semicolon, ";", line, column });
            break;

		case '"': {
            // store a string until a ending " else throw error
            std::string str = "";

            while (peek() != '"' && !is_at_end()) {
				char c = advance();

                // special string escape characters
				if (c == '\\') {
					if (is_at_end()){ 
						break;
					}

					char next = advance();

					switch (next) { 
					case 'n':
						str.push_back('\n');
						break;

					case 't':
						str.push_back('\t');
						break;

					case '"':
						str.push_back('"');
						break;

					case '\\':
						str.push_back('\\');
						break;

					default:
						str.push_back(next);
						break;
					}
				}
				else {
					str.push_back(c);
				}
            }

            if (is_at_end()) {
                return std::unexpected(
                    Error{
                        .code = ErrorCode::UNTERMINATED_STRING,
                        .message = "Unterminated string.",
                        .line = line,
                        .column = column,
                        .source_line = get_current_line(),
                    }
                    );
            }

            // consume closing "
            advance();
            tokens.emplace_back(Token{ TokenType::String, str, line, column });
            break;
        }

        default:
            if (is_digit(c)) {
                tokens.emplace_back(number());
                break;
            }

            if (is_alpha(c)) {
                tokens.emplace_back(identifier());
                break;
            }

            // unexpected token
            return std::unexpected(
                Error{
                    .code = ErrorCode::UNEXPECTED_CHAR,
                    .message = std::format(
                        "Unexpected character '{}'.",
                        c
                    ),
                    .line = line,
                    .column = column,
                    .source_line = get_current_line(),
                }
            );
        }
    }

    // final EOF
    tokens.emplace_back(Token{
        TokenType::Eof,
        "",
        _line,
        _column
        });

    return tokens;
}

Token Lexer::number()
{
    std::string value;

    value.push_back(previous());

    while (
        !is_at_end() &&
        (is_digit(peek()) || peek() == '.')
    )
    {
        value.push_back(advance());
    }

    return Token{
        TokenType::Number,
        value,
        _line,
        _column
    };
}

Token Lexer::identifier()
{
    std::string text;

    text.push_back(previous());

    while (is_alphanumeric(peek())) {
        text.push_back(advance());
    }

    // FUTURE: map of all keywords, loop over and find to create token
    if (text == "print") {
        return Token{
            TokenType::Print,
            text,
            _line,
            _column
        };
    }

    return Token{
        TokenType::Identifier,
        text,
        _line,
        _column,
    };
}

char Lexer::peek() const {
	if (is_at_end())
	{
		return '\0';
	}

	return _source[_current];
}

char Lexer::peek_next() const {
	if (_current + 1 >= _source.size())
	{
		return '\0';
	}

	return _source[_current + 1];
}

char Lexer::previous() const {
	if (_current == 0)
	{
		return '\0';
	}

	return _source[_current - 1];
}

char Lexer::advance()
{
    if (is_at_end()) return '\0';

    char c = _source.at(_current++);

    if (c == '\n') {
        ++_line;
        _column = 0;
    }
    else {
        ++_column;
    }

    return c;
}


bool Lexer::is_alpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::is_alphanumeric(char c) {
    return is_alpha(c) || is_digit(c);
}

bool Lexer::is_at_end() const {
	// current == max -> final char
	return _current == _max;
}

bool Lexer::is_digit(const char value)
{
	return std::isdigit(value); 
}

bool Lexer::is_whitespace(const char c)
{
	return std::isspace(static_cast<unsigned char>(c)); 
}

std::string Lexer::get_current_line() const
{
    if (_source.empty()) {
        return "";
    }

    size_t pos = std::min(_current, _source.size() - 1);
	size_t start = pos;

	while (start > 0 && _source[start - 1] != '\n') {
		--start;
	}

	size_t end = pos;

	while (end < _source.size() && _source[end] != '\n') {
		++end;
	}

	return _source.substr(start, end - start);
}
