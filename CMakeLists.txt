cmake_minimum_required(VERSION 4.1.2)

project(
    Interpreter
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp

    src/interpreter/Interpreter.cpp

    src/parser/Parser.cpp

    src/lexer/Lexer.cpp

    src/utils/Logger.cpp

)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
)
