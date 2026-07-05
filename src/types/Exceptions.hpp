#pragma once

#include <exception>
#include <string>

#include "types/Error.hpp"
#include "types/Token.hpp"

// Used within the Parser class.
class ParserException : public std::exception
{
  private:
    ErrorCode _error_code;
    std::string _message;

  public:
    explicit ParserException(ErrorCode error_code, std::string message)
        : _error_code(error_code), _message(std::move(message))
    {
    }

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

    [[nodiscard]] ErrorCode code() const noexcept
    {
        return _error_code;
    }
};

// Used within the Interpreter class
class RuntimeException : public std::exception
{
  private:
    ErrorCode _error_code;
    std::string _message;
    Token _token;

  public:
    explicit RuntimeException(ErrorCode error_code, std::string message, const Token& token)
        : _error_code(error_code), _message(std::move(message)), _token(token)
    {
    }

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

    [[nodiscard]] ErrorCode code() const noexcept
    {
        return _error_code;
    }

    [[nodiscard]] const Token& token() const noexcept
    {
        return _token;
    }
};
