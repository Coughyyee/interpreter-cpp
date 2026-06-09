module;
#include <string>

export module Token;

export enum class TokenType {
	Number,

	Bang,
	Plus,
	Minus,
	Multiply,
	Divide,

	LeftParen,
	RightParen,
	LeftBrace,
	RightBrace,

	Print,
	Identifier,

	Semicolon,

	Eof
};

export struct Token {
	TokenType type;
	std::string lexeme;

	size_t line;
	size_t column;
};


