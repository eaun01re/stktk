cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Declare project.
project(stktk VERSION 0.1)
set(PROJECT_DISPLAY_NAME "${PROJECT_NAME}")
add_definitions(-DPROJECT_NAME="${PROJECT_DISPLAY_NAME}")
add_definitions(-DPROJECT_NAME_FULL="${PROJECT_DISPLAY_NAME}")
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

    # Statically link the runtime libraries.
    # Not actually required since it doesn't link api-ms-win-core* libraries!
    # https://stackoverflow.com/a/54501156
    # https://social.msdn.microsoft.com/Forums/en-US/a28331ae-19a3-4a34-b3ba-1e8fd4430375/missing-apimswincore-dlls
    #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # Since 3.15.0
endif(MSVC)

add_subdirectory(game)
add_subdirectory(launcher)
