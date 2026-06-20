module;
#include <string>

export module Token;

export enum class TokenType {
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

	// Keywords End

	Semicolon,
	Comma,

	Eof
};

export struct Token {
	TokenType type;
	std::string lexeme;

	size_t line;
	size_t column;
};


