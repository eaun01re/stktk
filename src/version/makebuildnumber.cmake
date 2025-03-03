# Version generator.
#
# This script increments build number in `BuildNumber.txt`and writes this
# number and last commit hash into `VersionBuildRevision.h` after every build.
# To reset build number delete `BuildNumber.txt`.



# Setup version build.
set(BUILD_NUMBER_FILE "buildnumber.txt")
# Check for the previous build number.
if(EXISTS "${BUILD_NUMBER_FILE}")
    # Read build number from file.
    file(READ "${BUILD_NUMBER_FILE}" BUILD_NUMBER LIMIT 10)
else()
    # No file with build number, assuming very first build.
    set(BUILD_NUMBER 0)
endif()
# Increment build number.
math(EXPR BUILD_NUMBER "${BUILD_NUMBER} + 1")
# Save new build number into the same file.
file(WRITE "${BUILD_NUMBER_FILE}" "${BUILD_NUMBER}\n")



# Setup version revision.
set(BUILD_REVISION_HEADER "versionbuildrevision.h")
execute_process(COMMAND git log -1 --pretty=format:%h OUTPUT_VARIABLE REVISION)
if(REVISION STREQUAL "")
    set(REVISION "0")
endif()



# Write version header.
file(WRITE ${BUILD_REVISION_HEADER}
"// Generated automatically. Do not modify.\n\
#define PROJECT_VERSION_BUILD    ${BUILD_NUMBER}\n\
#define PROJECT_VERSION_REVISION \"${REVISION}\"\n")
