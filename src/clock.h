#pragma once


#include <chrono>


using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<double>;
using TimePoint = Clock::time_point;

Duration fromMicroseconds(signed long long microseconds);
