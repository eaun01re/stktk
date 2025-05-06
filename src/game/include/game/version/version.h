#pragma once


#include <string>
#include <game/version/version_revision_build.h>


#define STRINGIZE(A) #A
#define STR(A) STRINGIZE(A)

#define PROJECT_VERSION STR(PROJECT_VERSION_MAJOR.PROJECT_VERSION_MINOR.PROJECT_VERSION_REVISION.PROJECT_VERSION_BUILD)


extern std::string ProjectName;
extern std::string ProjectNameFull;
extern std::string ProjectCompanyName;
extern std::string ProjectLegalCopyright;
extern int         ProjectVersionMajor;
extern int         ProjectVersionMinor;
extern std::string ProjectVersionRevision;
extern int         ProjectVersionBuild;
extern std::string ProjectVersion;
