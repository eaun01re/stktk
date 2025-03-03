cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Declare project.
set(PROJECT_NAME stktk)
project(${PROJECT_NAME})
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DPROJECT_NAME_FULL="${PROJECT_NAME}")
add_definitions(-DPROJECT_COMPANY_NAME="bls")
add_definitions(-DPROJECT_LEGAL_COPYRIGHT="© 2025 bls")

# Version.
file(STRINGS "VERSION" VERSION_FILE)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION_FILE}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_FILE}")
add_definitions(-DPROJECT_VERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DPROJECT_VERSION_MINOR=${VERSION_MINOR})
message(STATUS "${PROJECT_NAME} v${VERSION_MAJOR}.${VERSION_MINOR}")

# Project sources.
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories(${SOURCE_DIR})
file(GLOB SOURCES_MAIN
    ${SOURCE_DIR}/*.cpp)
file(GLOB HEADERS_MAIN
    ${SOURCE_DIR}/*.h)

set(VERSION_DIR ${SOURCE_DIR}/version)
file(GLOB SOURCES_VERSION ${VERSION_DIR}/*.cpp)
file(GLOB HEADERS_VERSION ${VERSION_DIR}/*.h)

set(PROJECT_SOURCE_FILES
    ${SOURCES_MAIN}
    ${HEADERS_MAIN}
    ${SOURCES_VERSION}
    ${HEADERS_VERSION})

# Dependencies.
find_package(SFML 2.6 COMPONENTS graphics audio system REQUIRED)

# Add target.
add_executable(
    ${PROJECT_NAME} WIN32
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_RESOURCE_FILES})

# Link.
target_link_libraries(
    ${PROJECT_NAME}
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system)

# Compiler flags.
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wno-deprecated-declarations -Wno-comments -Wsuggest-override -Werror")
endif()
if(MSVC)
    # When GLFW is linked static set USE_MSVC_RUNTIME_LIBRARY_DLL to OFF.
    if(NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT /MTd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MT /MTd")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
    endif()
endif(MSVC)

# Increment version.
set(VERSION_GENERATOR VersionGenerator)
add_custom_target(
    ${VERSION_GENERATOR}
    COMMAND cmake -P ${VERSION_DIR}/makebuildnumber.cmake
    WORKING_DIRECTORY ${VERSION_DIR})
add_dependencies(${PROJECT_NAME} ${VERSION_GENERATOR})
