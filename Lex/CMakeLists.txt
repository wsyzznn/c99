cmake_minimum_required(VERSION 3.10)
project(SeuLex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SEULEX_SOURCES
    src/main.cpp
    src/lex_parser.cpp
    src/regex_converter.cpp
    src/nfa.cpp
    src/dfa.cpp
    src/code_generator.cpp
)

add_executable(seulex ${SEULEX_SOURCES})
target_include_directories(seulex PRIVATE include)

if (MSVC)
    target_compile_options(seulex PRIVATE /W4 /permissive-)
else()
    target_compile_options(seulex PRIVATE -Wall -Wextra -Wpedantic)
endif()
