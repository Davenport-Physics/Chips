cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Chips)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
message(STATUS "SDL2_DIR = ${SDL2_INCLUDE_DIRS}")

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/src/*h)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    string(APPEND CMAKE_C_FLAGS " -lm")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -lm")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -lm")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUGGING)
endif()

string(APPEND CMAKE_C_FLAGS " -O3")
string(APPEND CMAKE_C_FLAGS_RELEASE " -O3")

add_executable(chips ${sources} ${headers})
target_link_libraries(chips ${SDL2_LIBRARIES} "-lSDL2_mixer")

add_custom_command(TARGET chips PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/sounds
                   ${PROJECT_BINARY_DIR}/sounds
)
