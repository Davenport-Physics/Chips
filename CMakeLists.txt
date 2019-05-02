cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(Chips)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
message(STATUS "SDL2_DIR = ${SDL2_INCLUDE_DIRS}")

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/src/*h)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    string(APPEND CMAKE_C_FLAGS " -lm")
endif()

add_executable(chips ${sources} ${headers})
target_link_libraries(chips ${SDL2_LIBRARIES})
