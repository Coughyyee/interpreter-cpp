module;
#include <string>
#include <stdexcept>

export module Exceptions;

import Error;
import Token;

// Used within the Parser class.
export class ParserException : public std::exception {
private:
	ErrorCode _error_code;
	std::string _message;
public:
	ParserException(ErrorCode error_code, std::string message)
		: _error_code(error_code), _message(std::move(message)) {}

	const char* what() const noexcept override {
		return _message.c_str();
	}

	ErrorCode code() const {
		return _error_code;
	}
};

// Used within the Interpreter class
export class RuntimeException : public std::exception {
private: 
	ErrorCode _error_code;
	std::string _message;
	Token _token;

public:
	RuntimeException(ErrorCode error_code, std::string message, Token token)
		: _error_code(error_code), _message(std::move(message)), _token(std::move(token))
	{
	}

	const char* what() const noexcept override {
		return _message.c_str();
	}

	ErrorCode code() const {
		return _error_code;
	}

	const Token& token() const noexcept {
		return _token;
	}
};
