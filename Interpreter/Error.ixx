module;
#include <string>
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
	UNDEFINED_VARIABLE,
	ALREADY_DEFINED_VARIABLE,
	TYPE_MISMATCH,
	INVALID_OPERAND,
	INVALID_OPERANDS,
	DIVISION_BY_0,
	IS_NOT_TRUTHY,
};

export struct Error {
	ErrorCode code;
	std::string message;
	size_t line;
	size_t column;
	std::string source_line;
	// Add more in the future
};
