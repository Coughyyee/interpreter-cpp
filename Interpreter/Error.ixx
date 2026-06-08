module;
#include <string>
#include <stdexcept>

export module Error;

export enum ErrorCode {
	UNKNOWN = 0,
	BASIC = 1,
	COMPLEX = 2,
};

export struct Error {
	ErrorCode code;
	std::string message;
	size_t line;
	size_t column;
	std::string source_line;
	// Add more in the future
};

