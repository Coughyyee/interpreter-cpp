module;
#include <string>
#include <vector>
#include <ranges>
#include <stdexcept>
#include <format>
#include <expected>
#include <print>

module Lexer;

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
            tokens.emplace_back(Token{TokenType::Bang, "!", line, column});
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
