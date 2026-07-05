#pragma once

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "ast/Stmt.hpp"
#include "types/Error.hpp"
#include "types/Token.hpp"

class Parser
{
  private:
    const std::string& _source;
    std::vector<Token> _tokens;
    std::size_t _current{0};
    std::size_t _max; // uneeded?

  public:
    /**
     * @param source Source file in string format
     * @param tokens moves tokens into private member
     */
    explicit Parser(const std::string& source, std::vector<Token>&& tokens)
        : _source(source), _tokens(std::move(tokens)), _max(_tokens.size())
    {
    }

    std::expected<std::vector<std::unique_ptr<Stmt>>, Error> parse();

    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> loop_statement();
    std::unique_ptr<Stmt> if_statement();
    std::unique_ptr<Stmt> return_statement();
    std::unique_ptr<Stmt> function_declaration_statement();
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
    std::unique_ptr<Expr> postfix();
    std::unique_ptr<Expr> array();
    std::unique_ptr<Expr> primary();

  private:
    [[nodiscard]] bool is_at_end() const noexcept;
    [[nodiscard]] const Token& peek() const noexcept;
    [[nodiscard]] const Token& previous() const noexcept;
    Token advance();

    /**
     * @brief Check if current token matches parameter
     * @param type Token type to match against.
     * @return True and calls advance() if match else false no advance()
     */
    [[nodiscard]] bool match(TokenType type);
    [[nodiscard]] bool match(std::initializer_list<TokenType> types);
    [[nodiscard]] bool check(TokenType type) const noexcept;
    Token consume(TokenType type, ErrorCode code, const std::string& message);
    Token consume(std::initializer_list<TokenType> types, ErrorCode code, const std::string& message);
};
