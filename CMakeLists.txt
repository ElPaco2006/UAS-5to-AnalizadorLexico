cmake_minimum_required(VERSION 4.3)
project(PacoCompiler C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Lexer lexer.c
        lexer.h)

add_executable(PacoCompiler main.c)
target_link_libraries(PacoCompiler Lexer)

# Tests
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test Lexer GTest::gtest_main)