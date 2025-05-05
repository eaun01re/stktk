#pragma once


#include <chrono>
#include <filesystem>
#include <map>
#include <mutex>
#include <optional>
#include <sstream>


class Log final
{
public:
    enum Severity
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };
    using Timepoint = std::chrono::time_point<std::chrono::system_clock>;


public:
    static const std::map<Log::Severity, std::string> SEVERITY_DESCRIPTIONS;


public:
    /*!
     * Singleton instance accessor.
     * \return The reference to an instance of this class.
     */
    static Log& instance();

    void setPath(std::optional<std::filesystem::path> filename);

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
    ~Log();
    Log(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;
    Log& operator=(const Log&) = delete;


private:
    mutable std::mutex m_mutex;
    /// The name of the file with log.
    std::optional<std::filesystem::path> m_path;
    std::string m_filename;
};


inline std::ostream& operator<<(std::ostream &stream, Log::Severity severity)
{
    const auto it = Log::SEVERITY_DESCRIPTIONS.find(severity);
    const std::string severityStr = (it != Log::SEVERITY_DESCRIPTIONS.cend())
        ? it->second
        : "";
    return stream << severityStr;
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
