#include "Logger.hpp"

#include <print>

namespace logger
{
    void log(LogType type, const std::string& msg)
    {
        switch (type)
        {
        case LogType::Successfull:
            std::print("[Success]: {}\n", msg);
            break;
        case LogType::Error:
            std::print(stderr, "[Error]: {}\n", msg);
            break;
        }
    }

    void log(const Error& error)
    {
        const std::size_t space_length = error.column > 0 ? (error.column - 1) : 0;
        std::string error_arrow_spaces(space_length, ' ');
        std::string line_number_spaces(std::to_string(error.line).size(), ' ');
        // TODO: if "Unknown statement type." will just output goofy message, maybe create new overload without the line logging for this?

        // TODO: implement StageCode display
        std::println(stderr);
        std::println(stderr, "[Error Code {}]: {}", static_cast<int>(error.code), error.message);
        std::println(stderr, "Line: {} - Column: {}", error.line, error.column);
        std::println(stderr);
        std::println(stderr, "{} | {}", error.line, error.source_line.empty() ? "" : error.source_line);
        std::println(stderr, "{} | {}^", line_number_spaces, error_arrow_spaces);
    }
} // namespace logger
