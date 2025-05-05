cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Declare project.
project(stktk VERSION 0.1)
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DPROJECT_NAME_FULL="${PROJECT_NAME}")
add_definitions(-DPROJECT_COMPANY_NAME="bls")
add_definitions(-DPROJECT_LEGAL_COPYRIGHT="© 2025 bls")
add_definitions(-DPROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DPROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR})
message(STATUS "${PROJECT_NAME} v${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

# Compiler flags.
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
        -std=c++17 \
        -Wall \
        -Wextra \
        -Wno-deprecated-declarations \
        -Wno-comments \
        -Wsuggest-override \
        -Werror")
endif()
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

add_subdirectory(game)
add_subdirectory(launcher)
