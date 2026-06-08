#include <iostream>
#include <filesystem>
#include <fstream>
#include <expected>
#include <print>

import Logger;
import Lexer;

static std::expected<std::string, std::string> read_file(const char* path) {
	std::ifstream file(path);
	if (!file) {
		return std::unexpected("Unable to open file.");
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		Logger::log(LogType::Error, "Not enough arguments provided. Minimum required is 2.");
		return -1;
	}

	auto file = read_file(argv[1]);
	if (!file.has_value()) {
		Logger::log(LogType::Error, file.error());
		return -1;
	}

	Lexer lexer(file.value());
	auto lexer_result = lexer.scan_tokens();

	// lexer error
	if (!lexer_result.has_value()) {
		Logger::log(lexer_result.error());
		return -1;
	}

	for (auto& token : lexer_result.value()) {
		std::print("TokenType: {} | Lexeme: {}\n", static_cast<int>(token.type), token.lexeme);
	}

	std::print("\nLines: {} Column: {}", lexer.lines(), lexer.column());

	return 0;
}