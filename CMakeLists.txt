cmake_minimum_required(VERSION 3.16)

project(branchless_matching_engine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(matching_engine_lib
    src/matching_engine.cpp
)

target_include_directories(matching_engine_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(matching_engine_lib PRIVATE /W4 /permissive-)
else()
    target_compile_options(matching_engine_lib PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

add_executable(matching_demo
    src/main.cpp
)
target_link_libraries(matching_demo PRIVATE matching_engine_lib)

add_executable(matching_benchmark
    benchmarks/benchmark_branching_vs_branchless.cpp
)
target_link_libraries(matching_benchmark PRIVATE matching_engine_lib)

add_executable(matching_tests
    tests/test_matching_engine.cpp
)
target_link_libraries(matching_tests PRIVATE matching_engine_lib)

enable_testing()
add_test(NAME matching_tests COMMAND matching_tests)
