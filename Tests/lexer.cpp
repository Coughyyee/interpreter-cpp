#include "pch.h"

import Lexer;
import Token;

void token_match(std::string source, std::vector<TokenType> expected_types) {
	Lexer lexer(source);
	auto result = lexer.scan_tokens();
	ASSERT_TRUE(result.has_value());
	auto tokens = result.value();

	expected_types.push_back(TokenType::Eof); // Add Eof to expected types

	ASSERT_EQ(tokens.size(), expected_types.size());
	for (size_t i = 0; i < tokens.size(); ++i) {
		EXPECT_EQ(tokens[i].type, expected_types[i]);
	}
}

TEST(LexerTests, NumberLiteral) {
	token_match("12345;", { TokenType::NumberLiteral, TokenType::Semicolon });
}

TEST(LexerTests, StringLiteral) {
	token_match("\"Hello, World!\";", { TokenType::StringLiteral, TokenType::Semicolon });
}

TEST(LexerTests, Identifier) {
	token_match("myVariable;", { TokenType::Identifier, TokenType::Semicolon });
}

TEST(LexerTests, Operators) {
	token_match("=", { TokenType::Equal });
	token_match("!", { TokenType::Bang });
	token_match("!=", { TokenType::BangEqual });
	token_match(">", { TokenType::MoreThan});
	token_match(">=", { TokenType::MoreThanEqual });
	token_match("<", { TokenType::LessThan});
	token_match("<=", { TokenType::LessThanEqual });

	token_match("+", { TokenType::Plus});
	token_match("-", { TokenType::Minus});
	token_match("*", { TokenType::Multiply});
	token_match("/", { TokenType::Divide});

	token_match("(", { TokenType::LeftParen});
	token_match(")", { TokenType::RightParen});
	token_match("{", { TokenType::LeftBrace});
	token_match("}", { TokenType::RightBrace});

	token_match("->", { TokenType::Arrow});
}

TEST(LexerTests, Keywords) {
	// types
    token_match("number; string; bool;", {
		TokenType::Number, TokenType::Semicolon,
		TokenType::String, TokenType::Semicolon,
		TokenType::Bool, TokenType::Semicolon }
    );

	// output aka. print
    token_match("out;", {
		TokenType::Out, TokenType::Semicolon }
    );

	// variable declaration
    token_match("var;", {
		TokenType::Var, TokenType::Semicolon }
    );

	// bools
    token_match("true; false;", {
		TokenType::True, TokenType::Semicolon, TokenType::False, TokenType::Semicolon }
    );
}

TEST(LexerTests, VarDeclaration) {
    token_match("var x -> int = 10;", {
        TokenType::Var, TokenType::Identifier,
        TokenType::Arrow, TokenType::Identifier,
        TokenType::Equal, TokenType::NumberLiteral,
        TokenType::Semicolon }
        );
}

