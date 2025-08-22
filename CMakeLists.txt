cmake_minimum_required(VERSION 3.8)
project(cpplearn)

set(CMAKE_CXX_STANDARD 20)

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
