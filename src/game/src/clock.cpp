#include <game/clock.h>

#include <ctime>


Duration fromMicroseconds(signed long long microseconds)
{
    return std::chrono::duration_cast<Duration>(
        std::chrono::microseconds(microseconds));
}


std::string formatTime(
    const SystemTimePoint& value,
    const char* format)
{
    const auto time = std::chrono::system_clock::to_time_t(value);
    const tm *localtime = std::localtime(&time);
    char buffer[32];
    const std::size_t size =
        std::strftime(buffer, sizeof(buffer), format, localtime);
    return { buffer, size };
}


std::string formatTimeEscaped(const SystemTimePoint &timepoint)
{
    return formatTime(timepoint, "%Y-%d-%m_%H-%M-%S");
}
