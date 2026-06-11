module;
#include <string>

export module Token;

export enum class TokenType {
	Number,
	String,
	Identifier,

	Equal,

	Bang,
	BangEqual,
	EqualEqual,
	MoreThan,
	MoreThanEqual,
	LessThan,
	LessThanEqual,

	Plus,
	Minus,
	Multiply,
	Divide,

	LeftParen,
	RightParen,
	LeftBrace,
	RightBrace,

	Print,
	True,
	False,

	Semicolon,

	Eof
};

export struct Token {
	TokenType type;
	std::string lexeme;

	size_t line;
	size_t column;
};


