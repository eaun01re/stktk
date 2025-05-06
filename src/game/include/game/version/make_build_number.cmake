# Version generator.
#
# This script increments build number in `build_number.txt` and writes this
# number and last commit hash into `VersionBuildRevision.h` after every build.
# To reset build number delete `build_number.txt`.


# Set version revision.
execute_process(
    COMMAND git log -1 --pretty=format:%h
    OUTPUT_VARIABLE PROJECT_VERSION_REVISION)
if(PROJECT_VERSION_REVISION STREQUAL "")
    set(PROJECT_VERSION_REVISION "0")
endif()


# Set build number.
# NOTE: Windows resource file uses DWORD for storing version number.
# So max number is 65535.
set(BUILD_NUMBER_FILE "build_number.txt")
# Check for the previous build number.
if(EXISTS "${BUILD_NUMBER_FILE}")
    # Read build number from file.
    file(READ "${BUILD_NUMBER_FILE}" PROJECT_VERSION_BUILD LIMIT 10)
else()
    # No file with build number, assuming very first build.
    set(PROJECT_VERSION_BUILD 0)
endif()
# Increment build number.
math(EXPR PROJECT_VERSION_BUILD "${PROJECT_VERSION_BUILD} + 1")
# Save new build number into the same file.
file(WRITE "${BUILD_NUMBER_FILE}" "${PROJECT_VERSION_BUILD}")


# Write version header.
set(BUILD_REVISION_HEADER "version_revision_build.h")
file(WRITE ${BUILD_REVISION_HEADER}
"// Generated automatically. Do not modify.\n\
#define PROJECT_VERSION_REVISION ${PROJECT_VERSION_REVISION}\n\
#define PROJECT_VERSION_BUILD    ${PROJECT_VERSION_BUILD}\n\
")
