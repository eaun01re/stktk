#include <game/version/version.h>


const char  Separator = '.';
std::string ProjectName            = PROJECT_NAME;
std::string ProjectNameFull        = PROJECT_NAME_FULL;
std::string ProjectCompanyName     = PROJECT_COMPANY_NAME;
std::string ProjectLegalCopyright  = PROJECT_LEGAL_COPYRIGHT;
int         ProjectVersionMajor    = PROJECT_VERSION_MAJOR;
int         ProjectVersionMinor    = PROJECT_VERSION_MINOR;
std::string ProjectVersionRevision = PROJECT_VERSION_REVISION;
int         ProjectVersionBuild    = PROJECT_VERSION_BUILD;
std::string ProjectVersion         =
    std::to_string(ProjectVersionMajor) + Separator +
    std::to_string(ProjectVersionMinor) + Separator +
    ProjectVersionRevision + Separator +
    std::to_string(ProjectVersionBuild);
