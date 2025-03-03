#pragma once


#include <string>
#include "versionbuildrevision.h"


#define STRINGIZE(A) #A
#define STR(A) STRINGIZE(A)

#define PROJECT_VERSION_MAJOR_MINOR_BUILD STR(PROJECT_VERSION_MAJOR.PROJECT_VERSION_MINOR.PROJECT_VERSION_BUILD)
#define PROJECT_VERSION PROJECT_VERSION_MAJOR_MINOR_BUILD "." PROJECT_VERSION_REVISION


extern std::string ProjectName;
extern std::string ProjectNameFull;
extern std::string ProjectCompanyName;
extern std::string ProjectLegalCopyright;
extern int         ProjectVersionMajor;
extern int         ProjectVersionMinor;
extern int         ProjectVersionBuild;
extern std::string ProjectVersionRevision;
extern std::string ProjectVersion;
