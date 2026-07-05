#pragma once

#include <string>

enum class TokenType {
	NumberLiteral,
	StringLiteral,
	Identifier,

	// arrays
	NumberArray,
	StringArray,
	BoolArray,

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
	LeftBracket,
	RightBracket,

	// customer operators
	Arrow,

	// Keywords
	// Keyword lexeme map found in Lexer.ixx
	// Types
	Bool,
	Number,
	String,
	Void, // Functions only atm

	Out, // print
	Outln, // println
	Var,
	True,
	False,

	And,
	Or,

	Loop,
	If,
	Else,

	Typeof,

	Func,
	Return,

	// Keywords End

	Semicolon,
	Comma,

	Eof
};

struct Token {
	TokenType type;
	std::string lexeme;

	size_t line;
	size_t column;
};
