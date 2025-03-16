#include "clock.h"


Duration fromMicroseconds(signed long long microseconds)
{
    return std::chrono::duration_cast<Duration>(std::chrono::microseconds(microseconds));
}
