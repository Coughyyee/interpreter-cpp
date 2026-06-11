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
		size_t space_length = (error.column - 1);
		std::string error_arrow_spaces(space_length, ' ');
		std::string line_number_spaces(std::to_string(error.line).size(), ' ');

		// TODO: implement StageCode display
		std::println(stderr, "[Error Code {}]: {}", static_cast<int>(error.code), error.message);
		std::println(stderr, "Line: {} - Column: {}", error.line, error.column);
		std::println(stderr);
		std::println(stderr, "{} | {}", error.line, error.source_line.empty() ? "" : error.source_line);
		std::println(stderr, "{} | {}^", line_number_spaces, error_arrow_spaces);
	}
}
