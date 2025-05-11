#pragma once


#include <chrono>
#include <string>


using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<double>;
using TimePoint = Clock::time_point;
using SystemTimePoint = std::chrono::system_clock::time_point;

Duration fromMicroseconds(signed long long microseconds);

/*!
 * Converts the specified time point to a human-readable format.
 * \param[in] timePoint The time point to convert.
 * \param[in] format Pointer to a null-terminated multibyte character
 * string specifying the format of conversion.
 * \return The string representation of time point,
 * e.g. in yyyy.MM.dd HH:mm:ss format.
 */
std::string formatTime(
    const SystemTimePoint& timePoint = std::chrono::system_clock::now(),
    const char* format = "%d.%m.%Y %H:%M:%S");

/*!
 * Converts the specified time point to a human-readable format.
 * \param[in] timePoint The time point to convert.
 * \return The string representation of time point
 * in yyyy-MM-dd_HH-mm-ss format.
 */
std::string formatTimeEscaped(
    const SystemTimePoint& timePoint = std::chrono::system_clock::now());

/*!
 * Converts the specified time point to a human-readable format
 * with milliseconds.
 * \param[in] timePoint The time point to convert.
 * \return The string representation of time point.
 */
std::string formatTimeWithMilliseconds(
    const SystemTimePoint& timePoint = std::chrono::system_clock::now());
