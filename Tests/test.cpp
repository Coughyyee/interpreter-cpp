#include "pch.h"

import Lexer;
import Token;

TEST(LexerTests, NumberLiteral)
{
    Lexer lexer("123;");

    auto result = lexer.scan_tokens();

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(
        result.value()[0].type,
        TokenType::NumberLiteral
    );
}
