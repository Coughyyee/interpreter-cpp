module;
#include <iostream>
#include <string>
#include <print>

export module Logger;

import Error;

export enum class LogType { Successful, Error };

export namespace Logger {
	void log(LogType type, std::string_view msg) {
		switch (type) {
		case LogType::Successful:
			std::print("[Success]: {}\n", msg);
			break;
		case LogType::Error:
			std::print(stderr, "[Error]: {}\n", msg);
			break;
		}
	}

	void log(const Error& error) {
		size_t space_length = (error.column - 1) + std::string("[Line: ]: ").size() + std::to_string(error.line).size();
		std::string error_arrow_spaces(space_length, ' ');

		std::print(stderr, 
			"[Error Code {}]: {}\n\n[Line: {}]: {}\n{}^", 
			//error.column, 
			static_cast<int>(error.code),
			error.message, 
			error.line, 
			error.source_line.empty() ? "" : error.source_line,
			error_arrow_spaces
		);
	}
}
