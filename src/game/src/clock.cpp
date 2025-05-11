#include <game/clock.h>

#include <ctime>


Duration fromMicroseconds(signed long long microseconds)
{
    return std::chrono::duration_cast<Duration>(
        std::chrono::microseconds(microseconds));
}


std::string formatTime(
    const SystemTimePoint& timePoint,
    const char* format)
{
    const time_t time = std::chrono::system_clock::to_time_t(timePoint);
    const tm *localtime = std::localtime(&time);
    char buffer[32];

    const std::size_t size =
        std::strftime(buffer, sizeof(buffer), format, localtime);
    return { buffer, size };
}


std::string formatTimeEscaped(const SystemTimePoint &timePoint)
{
    return formatTime(timePoint, "%Y-%d-%m_%H-%M-%S");
}


std::string formatTimeWithMilliseconds(const SystemTimePoint &timePoint)
{
    const std::string formattedTime = formatTime(timePoint);
    const std::chrono::duration<double> timeSinceEpoch =
        timePoint.time_since_epoch();
    const std::chrono::milliseconds milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceEpoch);
    return formattedTime + '.' + std::to_string(milliseconds.count() % 1000);
}
