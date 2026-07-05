#include "parser/Parser.hpp"

#include "types/Exceptions.hpp"
#include "utils/SourceUtils.hpp"
#include <format>

std::expected<std::vector<std::unique_ptr<Stmt>>, Error> Parser::parse()
{
    try
    {
        std::vector<std::unique_ptr<Stmt>> statements;

        while (!is_at_end())
        {
            statements.push_back(statement());
        }

        return statements;
    }
    catch (const ParserException& err)
    {
        return std::unexpected(Error{
            .code = err.code(),
            .message = err.what(),
            .line = peek().line,
            .column = peek().column,
            .source_line = get_line_from_source(_source, peek().line),
        });
    }
}

std::unique_ptr<Stmt> Parser::statement()
{
    if (match(TokenType::Var))
    {
        return variable_declaration_statement();
    }

    if (match(TokenType::LeftBrace))
    {
        return block_statement();
    }

    if (match(TokenType::Out) || match(TokenType::Outln))
    {
        return print_statement();
    }

    if (match(TokenType::Loop))
    {
        return loop_statement();
    }

    if (match(TokenType::If))
    {
        return if_statement();
    }

    if (match(TokenType::Func))
    {
        return function_declaration_statement();
    }

    if (match(TokenType::Return))
    {
        return return_statement();
    }

    return expression_statement();
}

std::unique_ptr<Stmt> Parser::loop_statement()
{
    Token keyword = previous();

    std::optional<std::unique_ptr<Expr>> condition = std::nullopt; // default infinite loop

    // If a left brace isnt present after keyword, expecting a ( condition ).
    if (peek().type != TokenType::LeftBrace)
    {
        consume(TokenType::LeftParen, ErrorCode::EXPECTED,
                "Expected '(' after 'loop' if condition is to be presented.");

        condition = expression();

        consume(TokenType::RightParen, ErrorCode::EXPECTED, "Expected ')' after loop condition.");
    }

    // capture block
    auto block = statement();

    // todo: implement
    return std::make_unique<LoopStmt>(std::move(keyword), std::move(condition), std::move(block));
}

std::unique_ptr<Stmt> Parser::if_statement()
{
    auto keyword = previous(); // get 'if'

    consume(TokenType::LeftParen, ErrorCode::EXPECTED, "Expected '(' after 'if'.");

    auto condition = expression();

    consume(TokenType::RightParen, ErrorCode::EXPECTED, "Expected ')' after if condition.");

    auto thenBranch = statement();
    std::optional<std::unique_ptr<Stmt>> elseBranch = std::nullopt;

    if (match(TokenType::Else))
    {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(keyword), std::move(condition), std::move(thenBranch),
                                    std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::return_statement()
{
    Token keyword = previous();

    auto value = expression();

    consume(TokenType::Semicolon, ErrorCode::EXPECTED, "Expected ';' after value.");

    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::function_declaration_statement()
{
    Token keyword = previous();
    Token name = consume(TokenType::Identifier, ErrorCode::EXPECTED, "Expected function name.");

    consume(TokenType::LeftParen, ErrorCode::EXPECTED, "Expected '(' after function name.");

    std::vector<Parameter> parameters{};

    // parameter list
    while (!check(TokenType::RightParen) && !is_at_end())
    {
        auto param_name = consume(TokenType::Identifier, ErrorCode::EXPECTED, "Expected parameter name.");
        consume(TokenType::Arrow, ErrorCode::EXPECTED, "Expected '->' after parameter name.");
        auto param_type = consume(
            {
                TokenType::Bool,
                TokenType::Number,
                TokenType::String,
            },
            ErrorCode::EXPECTED, "Expected valid type name.");

        // Check for duplicate parameter names
        for (const auto& existing_param : parameters)
        {
            if (existing_param.name.lexeme == param_name.lexeme)
            {
                throw ParserException(ErrorCode::ALREADY_DEFINED_VARIABLE,
                                      std::format("Parameter '{}' is already defined.", param_name.lexeme));
            }
        }

        parameters.push_back(Parameter{std::move(param_name), std::move(param_type)});

        if (!match(TokenType::Comma))
        {
            break;
        }
    }

    consume(TokenType::RightParen, ErrorCode::EXPECTED, "Expected ')' after parameters.");

    // return type
    consume(TokenType::Arrow, ErrorCode::EXPECTED, "Expected '->' after parameters.");

    auto return_type = consume(
        {
            TokenType::Bool,
            TokenType::Number,
            TokenType::String,
            TokenType::Void, // Also accepts void return type
        },
        ErrorCode::EXPECTED, "Expected valid return type name.");

    // body
    auto body = statement();

    // return std::make_unique<VariableDeclarationStmt>(keyword, name, declared_type, std::move(expr));
    return std::make_unique<FunctionDeclarationStmt>(std::move(keyword), std::move(name), std::move(parameters),
                                                     std::move(return_type), std::move(body));
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
        ErrorCode::EXPECTED, "Expected valid type name.");

    bool is_array = false;
    if (peek().type == TokenType::LeftBracket)
    {
        advance(); // consume left bracket
        consume(   // consume right bracket
            TokenType::RightBracket, ErrorCode::EXPECTED, "Expected ']' after '[' in type definition.");
        is_array = true;
    }

    consume(TokenType::Equal, ErrorCode::EXPECTED, "Expected '=' after type.");

    auto expr = expression();

    consume(TokenType::Semicolon, ErrorCode::EXPECTED, "Expected ';' after expression.");

    if (is_array)
    {
        switch (declared_type.type)
        {
        case TokenType::Number:
            declared_type.type = TokenType::NumberArray;
            break;

        case TokenType::String:
            declared_type.type = TokenType::StringArray;
            break;

        case TokenType::Bool:
            declared_type.type = TokenType::BoolArray;
            break;
        default:
            // TODO: Implement sort of error handling
            break;
        }
    }

    return std::make_unique<VariableDeclarationStmt>(keyword, name, declared_type, std::move(expr));
}

std::unique_ptr<Stmt> Parser::print_statement()
{
    Token keyword = previous();

    bool new_line = false;
    if (keyword.type == TokenType::Outln)
    {
        new_line = true;
    }

    auto value = expression();

    consume(TokenType::Semicolon, ErrorCode::EXPECTED, "Expected ';' after value.");

    return std::make_unique<PrintStmt>(keyword, std::move(value), new_line);
}

std::unique_ptr<Stmt> Parser::expression_statement()
{
    Token start = peek();

    auto expr = expression();

    consume(TokenType::Semicolon, ErrorCode::EXPECTED, "Expected ';' after value.");

    return std::make_unique<ExpressionStmt>(start, std::move(expr));
}

std::unique_ptr<Stmt> Parser::block_statement()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RightBrace) && !is_at_end())
    {
        statements.push_back(statement());
    }

    consume(TokenType::RightBrace, ErrorCode::EXPECTED, "Expected '}' after block.");

    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::expression()
{
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
    auto expr = or_expression();

    if (match(TokenType::Equal))
    {
        auto value = assignment();

        if (auto variable = dynamic_cast<VariableExpr*>(expr.get()))
        {
            return std::make_unique<AssignmentExpr>(variable->name, std::move(value));
        }

        throw ParserException(ErrorCode::INVALID_ASSIGNMENT, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::or_expression()
{
    auto expr = and_expression();

    while (match(TokenType::Or))
    {
        auto op = previous();
        auto right = and_expression();
        expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::and_expression()
{
    auto expr = equality();

    while (match(TokenType::And))
    {
        auto op = previous();
        auto right = equality();
        expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality()
{
    auto expr = comparison();

    while (match(TokenType::EqualEqual) || match(TokenType::BangEqual))
    {
        Token op = previous();

        auto right = comparison();

        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (match(TokenType::MoreThan) || match(TokenType::MoreThanEqual) || match(TokenType::LessThan) ||
           match(TokenType::LessThanEqual))
    {
        Token op = previous();

        auto right = term();

        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term()
{
    auto expr = factor();

    // + -
    while (match(TokenType::Plus) || match(TokenType::Minus))
    {
        Token op = previous();

        auto right = factor();

        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor()
{
    auto expr = unary();

    // * /
    while (match(TokenType::Multiply) || match(TokenType::Divide))
    {
        Token op = previous();

        auto right = unary();

        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary()
{
    // - !
    if (match(TokenType::Minus) || match(TokenType::Bang))
    {
        Token op = previous();

        auto right = unary();

        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    if (match(TokenType::Typeof))
    {
        return std::make_unique<TypeOfExpr>(unary());
    }

    return postfix();
}

std::unique_ptr<Expr> Parser::postfix()
{
    auto expr = primary();

    while (true)
    {
        if (match(TokenType::LeftBracket))
        {
            auto index_expr = expression();

            consume(TokenType::RightBracket, ErrorCode::EXPECTED, "Expected ']' after index expression.");

            expr = std::make_unique<IndexExpr>(std::move(previous()), // token for postfix
                                               std::move(expr), std::move(index_expr));
        }
        else if (match(TokenType::LeftParen))
        {
            Token paren = previous();
            std::vector<std::unique_ptr<Expr>> arguments;

            // Parse arguments
            if (!check(TokenType::RightParen))
            {
                do
                {
                    arguments.push_back(expression());
                } while (match(TokenType::Comma));
            }

            consume(TokenType::RightParen, ErrorCode::EXPECTED, "Expected ')' after arguments.");

            expr = std::make_unique<CallExpr>(std::move(expr), std::move(paren), std::move(arguments));
        }
        else
        {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::array()
{
    std::vector<std::unique_ptr<Expr>> elements;

    if (!check(TokenType::RightBracket))
    {
        do
        {
            elements.push_back(expression());
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RightBracket, ErrorCode::EXPECTED, "Expected ']' after array.");

    return std::make_unique<ArrayExpr>(std::move(elements));
}

std::unique_ptr<Expr> Parser::primary()
{
    if (match(TokenType::Identifier))
    {
        return std::make_unique<VariableExpr>(previous());
    }

    if (match({TokenType::NumberLiteral, TokenType::StringLiteral, TokenType::True, TokenType::False}))
    {
        return std::make_unique<LiteralExpr>(previous());
    }

    // Group start (...)
    if (match(TokenType::LeftParen))
    {
        auto expr = expression();

        consume(TokenType::RightParen, ErrorCode::EXPECTED, "Expected ')' after expression.");

        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    if (match(TokenType::LeftBracket))
    {
        return array();
    }

    // Throw?
    throw ParserException(ErrorCode::UNKNOWN, "Parser Error.");
}

bool Parser::is_at_end() const noexcept
{
    return _tokens.at(_current).type == TokenType::Eof;
}

const Token& Parser::peek() const noexcept
{
    return _tokens.at(_current);
}

const Token& Parser::previous() const noexcept
{
    return _tokens.at(_current - 1);
}

Token Parser::advance()
{
    if (!is_at_end())
    {
        ++_current;
    }
    return previous();
}

bool Parser::match(TokenType type)
{
    if (!check(type))
    {
        return false;
    }

    advance();
    return true;
}

bool Parser::match(std::initializer_list<TokenType> types)
{
    for (auto type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(TokenType type) const noexcept
{
    return peek().type == type;
}

Token Parser::consume(TokenType type, ErrorCode code, const std::string& message)
{
    if (check(type))
    {
        return advance();
    }

    throw ParserException(code, std::format("{} Instead found '{}'.", message, peek().lexeme));
}

Token Parser::consume(std::initializer_list<TokenType> types, ErrorCode code, const std::string& message)
{
    for (auto type : types)
    {
        if (check(type))
        {
            return advance();
        }
    }

    throw ParserException(code, message);
}
