#include "pch.h"

import Lexer;
import Parser;
import Stmt;


// Statement tests

TEST(ParserTests, ParsePrintStatement)
{
    Lexer lexer("out 123;");

    auto tokens = lexer.scan_tokens();
    ASSERT_TRUE(tokens.has_value());

    Parser parser("out 123;", std::move(tokens.value()));

    auto result = parser.parse();

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().size(), 1);

    auto* stmt =
        dynamic_cast<PrintStmt*>(
            result.value()[0].get()
        );

    ASSERT_NE(stmt, nullptr);
}

TEST(ParserTests, ParseExpressionStatement) {
    Lexer lexer("10 + 10;");

    auto tokens = lexer.scan_tokens();
    ASSERT_TRUE(tokens.has_value());

    Parser parser("10 + 10;", std::move(tokens.value()));

    auto result = parser.parse();

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().size(), 1);

    auto* stmt =
        dynamic_cast<ExpressionStmt*>(
            result.value()[0].get()
        );

    ASSERT_NE(stmt, nullptr);
}

TEST(ParserTests, ParseVariableDeclarationStatement) {
    Lexer lexer("var x -> number = 10;");

    auto tokens = lexer.scan_tokens();
    ASSERT_TRUE(tokens.has_value());

    Parser parser("var x -> number = 10;", std::move(tokens.value()));

    auto result = parser.parse();

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().size(), 1);

    auto* stmt =
        dynamic_cast<VariableDeclarationStmt*>(
            result.value()[0].get()
        );

    ASSERT_NE(stmt, nullptr);
}
