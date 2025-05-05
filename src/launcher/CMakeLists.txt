set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Declare project.
project(stktk)

# Project sources.
set(SOURCE_DIR ${PROJECT_SOURCE_DIR})
include_directories(${SOURCE_DIR})
file(GLOB_RECURSE SOURCES_MAIN
    ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS_MAIN
    ${SOURCE_DIR}/*.h)

set(PROJECT_SOURCE_FILES ${SOURCES_MAIN} ${HEADERS_MAIN})

include_directories(${CMAKE_SOURCE_DIR}/../src/game/include)

# Add target.
add_executable(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_RESOURCE_FILES})

if(MSVC)
    option(BUILD_WITH_WINDOWSCRASHDUMP "Build game launcher with creating windows crash dump" OFF)
    if(BUILD_WITH_WINDOWSCRASHDUMP)
        add_definitions(-DBUILD_WITH_WINDOWSCRASHDUMP)
        set(BUILD_WITH_WINDOWSCRASHDUMP_LIB dbghelp)
    endif()
endif(MSVC)

# Link.
target_link_libraries(
    ${PROJECT_NAME}
    game
    ${BUILD_WITH_WINDOWSCRASHDUMP_LIB})

add_dependencies(${PROJECT_NAME} game)
