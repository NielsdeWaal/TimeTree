cmake_minimum_required(VERSION 3.13.4)

project(TimeTree LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_subdirectory(src)
add_library(TimeTree
    src/TimeTree.hpp
)

add_executable(main
    src/main.cpp
    src/TimeTree.hpp
)

target_link_libraries(
    main
    tl::expected
    fmt
)

add_subdirectory(test)

target_include_directories(TimeTree INTERFACE
    .
)

set_target_properties(TimeTree PROPERTIES LINKER_LANGUAGE CXX)

target_compile_options(TimeTree
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wmissing-include-dirs
    -Wswitch-enum
    -Wconversion
    -std=c++20
)

find_package(tl-expected REQUIRED)

target_link_libraries(
    TimeTree
    tl::expected
    fmt
    )
