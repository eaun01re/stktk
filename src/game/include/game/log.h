#pragma once


#include <array>
#include <filesystem>
#include <mutex>
#include <optional>
#include <sstream>


class Log final
{
public:
    enum Severity : std::uint8_t
    {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        Fatal = 5
    };

public:
    static const std::array<std::string, Severity::Fatal + 1> SEVERITY_DESCRIPTIONS;

public:
    /*!
     * Singleton instance accessor.
     * \return The reference to an instance of this class.
     */
    static Log& instance();

    void setPath(const std::optional<std::filesystem::path> &path);

    /*!
     * Writes the specified message to log.
     * \param message The message to write.
     * \param severity The severity of message.
     * \param file The name of file where this function was called.
     * \param line The number of line where this function was called.
     * \param functionName The name of function where this function was called.
     */
    void write(
        const std::string &message,
        const Severity severity = Severity::Info,
        const std::string &file = std::string(),
        const int line = 0,
        const std::string &functionName = std::string());

private:
    // Singleton part.
    Log(){}
    ~Log() = default;
    Log(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;
    Log& operator=(const Log&) = delete;

private:
    void writeToFile(
        const std::string &message,
        const Severity severity,
        const std::string &file,
        const int line,
        const std::string &functionName);

private:
    /// The name of the file with log.
    std::optional<std::filesystem::path> m_filename;
    unsigned int m_lastLogSizeCheck{ 0 };
    mutable std::mutex m_mutex;
};


inline std::ostream& operator<<(std::ostream &stream, Log::Severity severity)
{
    if (severity <= Log::Severity::Fatal)
    {
        stream << Log::SEVERITY_DESCRIPTIONS[severity];
    }
    return stream;
}


#define LOG_SEV(msg, severity) \
{\
    std::stringstream ss;\
    ss << msg;\
    Log::instance().write(ss.str(), severity);\
}
#define LOG_TRACE(msg) LOG_SEV(msg, Log::Severity::Trace)
#define LOG_DEBUG(msg) LOG_SEV(msg, Log::Severity::Debug)
#define LOG_INFO(msg) LOG_SEV(msg, Log::Severity::Info)
#define LOG_WARNING(msg) LOG_SEV(msg, Log::Severity::Warning)
#define LOG_ERROR(msg) LOG_SEV(msg, Log::Severity::Error)
#define LOG_FATAL(msg) LOG_SEV(msg, Log::Severity::Fatal)
