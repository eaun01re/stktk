#pragma once


#include <chrono>


using Duration = std::chrono::duration<double>;

double toSeconds(const Duration &time);
Duration fromMicroseconds(signed long long microseconds);
