module;
#include <string>
#include <stdexcept>

export module Exceptions;

import Error;

export class ParserException : public std::exception {
private:
	std::string _message;
	ErrorCode _error_code;
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

