#include <game/config.h>


Config& Config::instance()
{
    static Config singleton;
    return singleton;
}
