cmake_minimum_required(VERSION 3.16)
project(MathExpressionEvaluator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MEE_WERROR "Treat compiler warnings as errors" OFF)

# The evaluator itself, with no I/O: shared by the console program and tests.
add_library(mee_core STATIC
    Tokenizer.cpp
    ShuntingYard.cpp
    Evaluator.cpp
    Calculator.cpp)
target_include_directories(mee_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    # /EHsc is normally in CMake's default flags, but CI passes its own
    # CMAKE_CXX_FLAGS (for sanitizers), which replaces the defaults.
    # Without /EHsc, try/catch triggers warning C4530, an error under /WX.
    target_compile_options(mee_core PUBLIC /W4 /permissive- /EHsc)
    if(MEE_WERROR)
        target_compile_options(mee_core PUBLIC /WX)
    endif()
else()
    target_compile_options(mee_core PUBLIC -Wall -Wextra -Wpedantic -Wshadow)
    if(MEE_WERROR)
        target_compile_options(mee_core PUBLIC -Werror)
    endif()
endif()

add_executable(math_eval Main.cpp)
target_link_libraries(math_eval PRIVATE mee_core)

enable_testing()
add_executable(test_calculator tests/test_calculator.cpp)
target_link_libraries(test_calculator PRIVATE mee_core)
add_test(NAME calculator COMMAND test_calculator)
