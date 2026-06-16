#include "pch.h"
#include <expected>

import Lexer;
import Parser;
import Stmt;
import Error;


namespace
{
    std::expected<std::vector<std::unique_ptr<Stmt>>, Error>
        parse_source(const std::string& source)
    {
        Lexer lexer(source);

        auto tokens = lexer.scan_tokens();
        EXPECT_TRUE(tokens.has_value());

        if (!tokens.has_value())
        {
            return std::unexpected(tokens.error());
        }

        Parser parser(source, std::move(tokens.value()));

        return parser.parse();
    }

    template<typename T>
    T* get_stmt(
        const std::vector<std::unique_ptr<Stmt>>& statements,
        size_t index = 0
    )
    {
        return dynamic_cast<T*>(
            statements[index].get()
            );
    }
}

#pragma region Print stmt
TEST(ParserTests, ParsePrintStatement)
{
    auto result = parse_source(
        "out 123;"
    );

    ASSERT_TRUE(result.has_value());

    auto* stmt =
        get_stmt<PrintStmt>(
            result.value()
        );

    ASSERT_NE(stmt, nullptr);
}
#pragma endregion

#pragma region Expression stmt
TEST(ParserTests, ParseExpressionStatement)
{
    auto result = parse_source(
        "10 + 10;"
    );

    ASSERT_TRUE(result.has_value());

    auto* stmt =
        get_stmt<ExpressionStmt>(
            result.value()
        );

    ASSERT_NE(stmt, nullptr);
}
#pragma endregion

#pragma region var decl stmt
TEST(ParserTests, ParseVariableDeclarationStatement)
{
    auto result = parse_source(
        "var x -> number = 10;"
    );

    ASSERT_TRUE(result.has_value());

    auto* stmt =
        get_stmt<VariableDeclarationStmt>(
            result.value()
        );

    ASSERT_NE(stmt, nullptr);
}
#pragma endregion

#pragma region typeof 
TEST(ParserTests, ParseTypeOfNumber)
{
    auto result =
        parse_source(
            "out typeof 10;"
        );

    ASSERT_TRUE(result.has_value());

    auto* stmt =
        get_stmt<PrintStmt>(
            result.value()
        );

    ASSERT_NE(stmt, nullptr);
}

TEST(ParserTests, ParseTypeOfVariable)
{
    auto result =
        parse_source(
            R"(
            var x -> number = 10;
            out typeof x;
            )"
        );

    ASSERT_TRUE(result.has_value());

    ASSERT_EQ(
        result.value().size(),
        2
    );
}

TEST(ParserTests, ParseTypeOfExpression)
{
    auto result =
        parse_source(
            "out typeof (10 + 10);"
        );

    ASSERT_TRUE(result.has_value());
}
#pragma endregion

#pragma region assignment
TEST(ParserTests, ParseAssignment)
{
    auto result =
        parse_source(
            R"(
            var x -> number = 10;
            x = 20;
            )"
        );

    ASSERT_TRUE(result.has_value());

    ASSERT_EQ(
        result.value().size(),
        2
    );
}
#pragma endregion

#pragma region if stmt
TEST(ParserTests, ParseIfStatement)
{
    auto result =
        parse_source(
            R"(
            if (true)
            {
                out 10;
            }
            )"
        );

    ASSERT_TRUE(result.has_value());

    auto* stmt =
        get_stmt<IfStmt>(
            result.value()
        );

    ASSERT_NE(stmt, nullptr);
}

TEST(ParserTests, ParseIfElseStatement)
{
    auto result =
        parse_source(
            R"(
            if (true)
            {
                out 10;
            }
            else
            {
                out 20;
            }
            )"
        );

    ASSERT_TRUE(result.has_value());

    auto* stmt =
        get_stmt<IfStmt>(
            result.value()
        );

    ASSERT_NE(stmt, nullptr);
}

TEST(ParserTests, ParseNestedIfStatement)
{
    auto result =
        parse_source(
            R"(
            if (true)
            {
                if (false)
                {
                    out 1;
                }
            }
            )"
        );

    ASSERT_TRUE(result.has_value());
}
#pragma endregion

#pragma region while loop
TEST(ParserTests, ParseWhileLoop)
{
    auto result =
        parse_source(
            R"(
            loop (true)
            {
                out 10;
            }
            )"
        );

    ASSERT_TRUE(result.has_value());

    auto* stmt =
        get_stmt<LoopStmt>(
            result.value()
        );

    ASSERT_NE(stmt, nullptr);
}

TEST(ParserTests, ParseNestedWhileLoop)
{
    auto result =
        parse_source(
            R"(
            loop (true)
            {
                loop (false)
                {
                    out 1;
                }
            }
            )"
        );

    ASSERT_TRUE(result.has_value());
}
#pragma endregion

#pragma region errorneous tests
TEST(ParserTests, MissingSemicolon)
{
    auto result =
        parse_source(
            "out 10"
        );

    ASSERT_FALSE(result.has_value());
}

TEST(ParserTests, MissingClosingParen)
{
    auto result =
        parse_source(
            "out (10 + 10;"
        );

    ASSERT_FALSE(result.has_value());
}

TEST(ParserTests, InvalidAssignmentTarget)
{
    auto result =
        parse_source(
            "(10 + 10) = 20;"
        );

    ASSERT_FALSE(result.has_value());
}

TEST(ParserTests, InvalidVariableType)
{
    auto result =
        parse_source(
            "var x -> potato = 10;"
        );

    ASSERT_FALSE(result.has_value());
}
#pragma endregion

