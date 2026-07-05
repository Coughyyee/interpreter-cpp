#include "lexer/Lexer.hpp"
#include "utils/SourceUtils.hpp"
#include <format>

std::expected<std::vector<Token>, Error> Lexer::scan_tokens()
{
    std::vector<Token> tokens;

    while (!is_at_end())
    {
        size_t line = _line;
        size_t column = _column + 1;

        char c = advance();

        if (is_whitespace(c))
            continue;

        switch (c)
        {
        case '!':
            if (peek() == '=')
            {
                advance();
                tokens.emplace_back(Token{TokenType::BangEqual, "!=", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::Bang, "!", line, column});
            break;
        case '=':
            if (peek() == '=')
            {
                advance();
                tokens.emplace_back(Token{TokenType::EqualEqual, "==", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::Equal, "=", line, column});
            break;
        case '>':
            if (peek() == '=')
            {
                advance();
                tokens.emplace_back(Token{TokenType::MoreThanEqual, ">=", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::MoreThan, ">", line, column});
            break;
        case '<':
            if (peek() == '=')
            {
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
            if (peek() == '>')
            {
                advance();
                tokens.emplace_back(Token{TokenType::Arrow, "->", line, column});
                break;
            }
            tokens.emplace_back(Token{TokenType::Minus, "-", line, column});
            break;

        case '*':
            tokens.emplace_back(Token{TokenType::Multiply, "*", line, column});
            break;

        case '/':
            // inline comments
            if (peek() == '/')
            {
                advance(); // consume

                while (!is_at_end() && peek() != '\n')
                    advance();

                break;
            }

            // divide operator
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

        case '[':
            tokens.emplace_back(Token{TokenType::LeftBracket, "[", line, column});
            break;

        case ']':
            tokens.emplace_back(Token{TokenType::RightBracket, "]", line, column});
            break;

        case ';':
            tokens.emplace_back(Token{TokenType::Semicolon, ";", line, column});
            break;
        case ',':
            tokens.emplace_back(Token{TokenType::Comma, ",", line, column});
            break;

        case '"':
        {
            // store a string until a ending " else throw error
            std::string str = "";

            while (!is_at_end() && peek() != '"')
            {
                char c = advance();

                // special string escape characters
                if (c == '\\')
                {
                    if (is_at_end())
                    {
                        break;
                    }

                    char next = advance();

                    switch (next)
                    {
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
                else
                {
                    str.push_back(c);
                }
            }

            if (is_at_end())
            {
                return std::unexpected(Error{
                    .code = ErrorCode::UNTERMINATED_STRING,
                    .message = "Unterminated string.",
                    .line = line,
                    .column = column,
                    .source_line = get_line_from_source(_source, line),
                });
            }

            // consume closing "
            advance();
            tokens.emplace_back(Token{TokenType::StringLiteral, str, line, column});
            break;
        }

        default:
            if (is_digit(c))
            {
                tokens.emplace_back(number());
                break;
            }

            if (is_alpha(c))
            {
                tokens.emplace_back(identifier());
                break;
            }

            // unexpected token
            return std::unexpected(Error{
                .code = ErrorCode::UNEXPECTED_CHAR,
                .message = std::format("Unexpected character '{}'.", c),
                .line = line,
                .column = column,
                .source_line = get_line_from_source(_source, line),
            });
        }
    }

    // final EOF
    tokens.emplace_back(Token{TokenType::Eof, "", _line, _column});

    return tokens;
}

Token Lexer::number()
{
    std::string value;

    value.push_back(previous());

    while (!is_at_end() && (is_digit(peek()) || peek() == '.'))
    {
        value.push_back(advance());
    }

    return Token{TokenType::NumberLiteral, value, _line, _column};
}

Token Lexer::identifier()
{
    std::string text;

    text.push_back(previous());

    while (!is_at_end() && is_alphanumeric(peek()))
    {
        text.push_back(advance());
    }

    // iterate over map and return corrisponding token if found. Else return identifier token.
    if (auto it = _keywords.find(text); it != _keywords.end())
    {
        return Token{it->second, text, _line, _column};
    }

    return Token{
        TokenType::Identifier,
        text,
        _line,
        _column,
    };
}

char Lexer::peek() const
{
    if (is_at_end())
    {
        return '\0';
    }

    return _source[_current];
}

char Lexer::peek_next() const
{
    if (_current + 1 >= _source.size())
    {
        return '\0';
    }

    return _source[_current + 1];
}

char Lexer::previous() const
{
    if (_current == 0)
    {
        return '\0';
    }

    return _source[_current - 1];
}

char Lexer::advance()
{
    if (is_at_end())
        return '\0';

    char c = _source.at(_current++);

    if (c == '\n')
    {
        ++_line;
        _column = 0;
    }
    else
    {
        ++_column;
    }

    return c;
}

constexpr bool Lexer::is_alpha(char c) noexcept
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

constexpr bool Lexer::is_digit(const char c) noexcept
{
    return c >= '0' && c <= '9';
}

constexpr bool Lexer::is_alphanumeric(char c) noexcept
{
    return is_alpha(c) || is_digit(c);
}

constexpr bool Lexer::is_whitespace(const char c) noexcept
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

constexpr bool Lexer::is_at_end() const noexcept
{
    // current == max -> final char
    return _current >= _source.size();
}
