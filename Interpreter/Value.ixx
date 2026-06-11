module;
#include <variant>
#include <string>
export module Value;

// C++ types used within the interpreter.
export using Value = std::variant<double, bool, std::string>;
