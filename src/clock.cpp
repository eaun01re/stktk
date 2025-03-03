#include "clock.h"


double toSeconds(const Duration &time)
{
    return time.count();
}


Duration fromMicroseconds(signed long long microseconds)
{
    return std::chrono::duration_cast<Duration>(std::chrono::microseconds(microseconds));
}
