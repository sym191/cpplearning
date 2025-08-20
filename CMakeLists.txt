cmake_minimum_required(VERSION 3.8)
project(cpplearn)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.h" "src/*.h")

add_executable(main
        ${SOURCES}
        )