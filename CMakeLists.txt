cmake_minimum_required(VERSION 3.13)
project(Racket_Interpreter)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)

if (Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif ()

add_executable(racquet src/main.cpp src/interpret/parser.cpp src/interpret/parser.h src/functions/functions.cpp src/functions/functions.h src/expressions/expressions.cpp src/expressions/expressions.h src/expressions/partial_expression.cpp src/expressions/tuple_expression.cpp src/expressions/function_expressions.cpp src/functions/boolean_operations.cpp src/functions/math_functions.cpp src/interpret/interpret.cpp src/interpret/interpret.h src/functions/testing_functions.cpp)

if (Boost_FOUND)

    target_link_libraries(racquet ${Boost_LIBRARIES})

endif ()