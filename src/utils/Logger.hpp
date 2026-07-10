#pragma once

#include "types/Error.hpp"
#include <cstdint>
#include <string>

enum class LogType : std::uint8_t
{
    Successfull,
    Error
};

namespace logger
{
    void log(LogType type, const std::string& msg);
    void log(const Error& error);
} // namespace logger
