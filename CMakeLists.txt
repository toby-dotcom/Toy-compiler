cmake_minimum_required(VERSION 3.10)
project(ToyCompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(toy_compiler
    main.cpp
    parser/pars.cpp
)

target_include_directories(toy_compiler PRIVATE
    lexer
    parser
)

