cmake_minimum_required(VERSION 3.13)
project(Racket_Interpreter)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.57.0 COMPONENTS system filesystem program_options thread REQUIRED)

if (APPLE)

    LINK_DIRECTORIES(/usr/local/lib)

endif ()

find_library(GMP NAMES libgmp.dylib REQUIRED)

if(GMP)
    message(STATUS "GMP: ${GMP}")
endif ()

if (Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif ()

add_executable(racquet src/main.cpp src/interpret/parser.cpp src/interpret/parser.h src/functions/functions.cpp
        src/functions/functions.h src/expressions/expressions.cpp src/expressions/expressions.h
        src/expressions/partial_expression.cpp src/expressions/tuple_expression.cpp
        src/expressions/function_expressions.cpp src/functions/boolean_operations.cpp
        src/functions/math_functions.cpp src/interpret/interpret.cpp src/interpret/interpret.h
        src/functions/testing_functions.cpp src/interpret/repl.cpp src/args/args.cpp
        src/args/args.h src/functions/symbol_functions.cpp src/functions/string_functions.cpp
        src/expressions/struct_expression.cpp src/expressions/struct_expression.h src/expressions/list_expression.cpp
        src/functions/list_functions.cpp)


target_link_libraries(racquet ${Boost_LIBRARIES} ${GMP})
