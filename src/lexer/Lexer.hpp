#pragma once

#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

#include "types/Error.hpp"
#include "types/Token.hpp"

/**
 * @brief Involves taking the source and producing tokens from source
 */
class Lexer
{
  private:
    const std::string& _source;
    std::size_t _current{0};
    std::size_t _line{1};
    std::size_t _column{0};

    // all keywords and their corresponding token types
    const std::unordered_map<std::string, TokenType> _keywords{
        {"out", TokenType::Out},       {"outln", TokenType::Outln},

        {"var", TokenType::Var},

        {"true", TokenType::True},     {"false", TokenType::False},

        {"bool", TokenType::Bool},     {"number", TokenType::Number}, {"string", TokenType::String},
        {"void", TokenType::Void},

        {"loop", TokenType::Loop},     {"if", TokenType::If},         {"else", TokenType::Else},
        {"and", TokenType::And},       {"or", TokenType::Or},

        {"func", TokenType::Func},     {"return", TokenType::Return},

        {"typeof", TokenType::Typeof},
    };

  public:
    explicit Lexer(const std::string& source) : _source(source) {}

    std::expected<std::vector<Token>, Error> scan_tokens();

  private:
    Token number();
    Token identifier();

    [[nodiscard]] char peek() const;
    [[nodiscard]] char peek_next() const;
    [[nodiscard]] char previous() const;
    char advance();

    /*  static helpers - dont depend on class state */
    [[nodiscard]] static constexpr bool is_alpha(char c) noexcept;
    [[nodiscard]] static constexpr bool is_digit(const char c) noexcept;
    [[nodiscard]] static constexpr bool is_alphanumeric(char c) noexcept;
    // bool is_numeric(const std::string& value) { return std::all_of(value.begin(), value.end(), is_digit); }
    [[nodiscard]] static constexpr bool is_whitespace(const char c) noexcept;

    [[nodiscard]] constexpr bool is_at_end() const noexcept;
};
