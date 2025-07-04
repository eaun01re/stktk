set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Declare project.
project(game)

# Project sources.
set(SOURCE_DIR ${PROJECT_SOURCE_DIR})
include_directories(${SOURCE_DIR})
file(GLOB_RECURSE SOURCES_MAIN
    ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS_MAIN
    ${SOURCE_DIR}/*.h)

# Timer generator.
set(RENDER_TIMER AUTO CACHE STRING "Timer interval generator")
set_property(
    CACHE RENDER_TIMER
    PROPERTY STRINGS
    SFML # sf::Clock
    STD # std::chrono::high_resolution_clock
    FIXED # Fixed update interval. For debugging.
    )
if(RENDER_TIMER STREQUAL "SFML")
    add_definitions(-DUSE_SFML_CLOCK)
elseif(RENDER_TIMER STREQUAL "STD")
    add_definitions(-DUSE_STD_CLOCK)
else()
    add_definitions(-DUSE_FIXED_CLOCK)
endif()

set(PROJECT_SOURCE_FILES
    ${SOURCES_MAIN}
    ${HEADERS_MAIN})

# Dependencies.
find_package(SFML 2.6 COMPONENTS graphics audio system REQUIRED)
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
if(Boost_FOUND)
    message(STATUS "Found BOOST ${Boost_VERSION_STRING}")
    message(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}")
endif()

# Add target.
add_library(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_RESOURCE_FILES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS}
    PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Link.
target_link_libraries(
    ${PROJECT_NAME}
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
    ${BOOST_LIBRARY_DIRS})

# Increment version.
set(VERSION_GENERATOR VersionGenerator)
set(VERSION_DIR ${SOURCE_DIR}/include/game/version/)
add_custom_target(
    ${VERSION_GENERATOR}
    COMMAND cmake -P ${VERSION_DIR}/make_build_number.cmake
    WORKING_DIRECTORY "${VERSION_DIR}")
add_dependencies(${PROJECT_NAME} ${VERSION_GENERATOR})
