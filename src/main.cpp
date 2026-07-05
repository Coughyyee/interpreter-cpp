#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>

#include "utils/Logger.hpp"

// import Lexer;
#include "lexer/Lexer.hpp"
// import Parser;
#include "parser/Parser.hpp"
// import AstPrinter;
#include "utils/AstPrinter.hpp"
// import Interpreter;
#include "interpreter/Interpreter.hpp"

namespace
{
    std::expected<std::string, std::string> read_file(const char* path)
    {
        std::ifstream file(path);
        if (!file)
        {
            return std::unexpected("Unable to open file.");
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }
} // namespace

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        logger::log(LogType::Error, "Not enough arguments provided. Minimum required is 2.");
        return -1;
    }

    auto file = read_file(argv[1]);
    if (!file)
    {
        logger::log(LogType::Error, file.error());
        return -1;
    }

    auto source = file.value();

    Lexer lexer(source);
    auto tokens = lexer.scan_tokens();

    // lexer error
    if (!tokens)
    {
        logger::log(tokens.error());
        return -1;
    }

    Parser parser(source, std::move(tokens.value()));
    auto ast = parser.parse();

    // parser error
    if (!ast)
    {
        logger::log(ast.error());
        return -1;
    }

    AstPrinter printer;

    std::println("AST Printer:");
    for (auto& stmt : ast.value())
    {
        std::println("{}", printer.print(stmt.get()));
    }

    std::println();
    std::println("-------");
    std::println();
    std::println("Program output:");

    Interpreter interpreter(source);

    for (auto& stmt : ast.value())
    {
        auto result = interpreter.interpret(stmt.get());

        // interpreter error
        if (!result)
        {
            logger::log(result.error());
            return -1;
        }
    }

    // for (auto& token : tokens.value())
    // {
    //     std::print("TokenType: {} | Lexeme: {}\n", static_cast<int>(token.type), token.lexeme);
    // }

    return 0;
}
