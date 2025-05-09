#include <fstream>
#include <iomanip>
#include <iostream>

#include <game/log.h>
#include <game/clock.h>


namespace
{

const std::string LOG_FILE_NAME = "log";
const std::string LOG_FILE_EXTENSION = ".txt";
constexpr unsigned int CHECK_LOG_SIZE_PERIOD = 100;
constexpr std::uintmax_t MAX_LOG_SIZE = 1024 * 1024 * 9; // Bytes.


void writeToStream(
    std::ostream &stream,
    const std::string &message,
    const Log::Severity severity,
    const std::string &file,
    const int line,
    const std::string &functionName)
{
    std::ignore = file;
    std::ignore = line;
    std::ignore = functionName;

    stream
        << formatTime() << ' '
        << std::setw(7) << std::right << severity << ": "
        << message
        << std::endl << std::flush;
}


std::filesystem::path makeLogFilename(const std::filesystem::path &path)
{
    return path / std::filesystem::path(LOG_FILE_NAME + LOG_FILE_EXTENSION);
}


void checkLogSize(const std::filesystem::path &filename)
{
    const std::uintmax_t size = std::filesystem::file_size(filename);
    if (size >= MAX_LOG_SIZE)
    {
        std::filesystem::rename(
            filename,
            LOG_FILE_NAME + "_upto_" + formatTimeEscaped()
                + LOG_FILE_EXTENSION);
    }
}

}


const std::array<std::string, Log::Severity::Fatal + 1> Log::SEVERITY_DESCRIPTIONS =
{
    "Trace",
    "Debug",
    "Info",
    "Warning",
    "Error",
    "Fatal"
};


Log& Log::instance()
{
    static Log singleton;
    return singleton;
}


void Log::setPath(const std::optional<std::filesystem::path> &path)
{
    if (!path.has_value())
    {
        m_filename = std::nullopt;
        return;
    }

    m_filename = makeLogFilename(path.value());
}


void Log::write(
    const std::string &message,
    const Severity severity,
    const std::string &file,
    const int line,
    const std::string &functionName)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    std::ostream &stream = severity <= Log::Severity::Warning
        ? std::cout
        : std::cerr;
    writeToStream(stream, message, severity, file, line, functionName);

    writeToFile(message, severity, file, line, functionName);
}


void Log::writeToFile(
    const std::string &message,
    const Log::Severity severity,
    const std::string &file,
    const int line,
    const std::string &functionName)
{
    if (!m_filename.has_value())
    {
        return;
    }

    if (CHECK_LOG_SIZE_PERIOD == 0 ||
        ++m_lastLogSizeCheck == CHECK_LOG_SIZE_PERIOD)
    {
        checkLogSize(m_filename.value());
        m_lastLogSizeCheck = 0;
    }

    std::ofstream fstream(m_filename.value(), std::ios::app);
    writeToStream(fstream, message, severity, file, line, functionName);
    fstream.close();
}
