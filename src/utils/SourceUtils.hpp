#pragma once

#include <string>

/**
 * @brief Returns the contents of a specific line from the source code.
 *
 * Lines are numbered starting at 1. If the requested line does not exist,
 * an empty string is returned.
 *
 * @param source The complete source code.
 * @param line The 1-based line number to retrieve.
 * @return The contents of the requested line.
 */
[[nodiscard]]
inline std::string get_line_from_source(const std::string& source, size_t line)
{
    std::size_t current_line = 1;
    std::size_t start = 0;

    for (std::size_t i = 0; i < source.size(); ++i)
    {
        if (current_line == line)
        {
            start = i;
            break;
        }

        if (source[i] == '\n')
        {
            ++current_line;
        }
    }

    std::size_t end = start;
    while (end < source.size() && source[end] != '\n')
    {
        ++end;
    }

    return source.substr(start, end - start);
}
