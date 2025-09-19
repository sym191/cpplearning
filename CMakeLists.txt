cmake_minimum_required(VERSION 3.8)
project(cpplearn)

set(CMAKE_CXX_STANDARD 20)

# 可选的优化级别设置，默认为 O0（无优化）
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    FORCE
  )
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug build flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "Release build flags" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING "Release with debug info build flags" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os" CACHE STRING "Minimal size build flags" FORCE)

# 全局编译选项
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -pthread
)

# 链接选项
link_libraries(-pthread)

file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.h" "src/*.h")
include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
        GIT_SHALLOW TRUE 
)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-30-2
        GIT_SHALLOW TRUE 
)

FetchContent_MakeAvailable(asio)


add_executable(${PROJECT_NAME}
        ${SOURCES}
        )
target_include_directories(${PROJECT_NAME} PRIVATE ${spdlog_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${asio_SOURCE_DIR}/asio/include)
# target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/so/libspdlog.a)
