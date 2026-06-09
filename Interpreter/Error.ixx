module;
#include <string>
#include <stdexcept>

export module Error;

export enum ErrorCode {
	UNKNOWN = 0,

	UNEXPECTED_CHAR,
	EXPECTED,
};

export struct Error {
	ErrorCode code;
	std::string message;
	size_t line;
	size_t column;
	std::string source_line;
	// Add more in the future
};

export class ParserException : public std::exception {
private:
	const std::string& _message;
	ErrorCode _error_code;
public:
	ParserException(ErrorCode error_code, const std::string& message)
		: _error_code(error_code), _message(message) {}

	const char* what() const noexcept override {
		return _message.c_str();
	}

	ErrorCode code() const {
		return _error_code;
	}
};

