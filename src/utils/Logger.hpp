#pragma once

#include <string>
#include "types/Error.hpp"

enum class LogType { Successfull, Error };

namespace logger
{
    void log(LogType type, const std::string& msg);
    void log(const Error& error);
}
