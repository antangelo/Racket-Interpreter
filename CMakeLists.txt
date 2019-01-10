cmake_minimum_required(VERSION 3.13)
project(Racket_Interpreter)

set(CMAKE_CXX_STANDARD 14)

add_executable(Racket_Interpreter src/main.cpp src/parser.cpp src/parser.h src/functions.cpp src/functions.h src/expressions.cpp src/expressions.h)