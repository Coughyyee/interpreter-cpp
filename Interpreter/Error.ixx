module;
#include <string>
#include <stdexcept>

export module Error;

// Interpreter stages for error reporting.
export enum StageCode {
	// TODO: implement
	READING_SOURCE = 0, // A
	LEXER, // B
	PARSER, // C
	INTERPRETER, // D
};

// Error codes for different error types.
export enum ErrorCode {
	UNKNOWN = 0,

	UNEXPECTED_CHAR,
	EXPECTED,
	INVALID_ASSIGNMENT,
	UNTERMINATED_STRING,
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

