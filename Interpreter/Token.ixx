module;
#include <string>

export module Token;

export enum class TokenType {
	NumberLiteral,
	StringLiteral,
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

	// customer operators
	Arrow,

	// Keywords
	// Keyword lexeme map found in Lexer.ixx
	// Types
	Bool,
	Number,
	String,

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

	// Keywords End

	Semicolon,

	Eof
};

export struct Token {
	TokenType type;
	std::string lexeme;

	size_t line;
	size_t column;
};


