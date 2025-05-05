#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <game/log.h>
#include <game/clock.h>


namespace
{

const std::string LOG_FILENAME = "log";
constexpr int MAX_LOG_SIZE = 1024 * 1024 * 9;


void write(
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
        << std::setw(7) << std::left << severity << ": "
        << message
        << std::endl << std::flush;
}


std::string makeLogFilename(const std::filesystem::path &path)
{
    return (path / std::filesystem::path(LOG_FILENAME)).string();
}


void checkLogSize(const std::string &filename, const int sizeMax = MAX_LOG_SIZE)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    const int size = in.tellg();
    in.close();
    if (size >= sizeMax)
    {
        std::filesystem::rename(
            filename,
            filename + "_upto_" + formatTimeEscaped());
    }
}

}


const std::map<Log::Severity, std::string> Log::SEVERITY_DESCRIPTIONS =
{
    { Log::Severity::Trace, "Trace" },
    { Log::Severity::Debug, "Debug" },
    { Log::Severity::Info, "Info" },
    { Log::Severity::Warning, "Warning" },
    { Log::Severity::Error, "Error" },
    { Log::Severity::Fatal, "Fatal" }
};


Log& Log::instance()
{
    static Log singleton;
    return singleton;
}


void Log::setPath(std::optional<std::filesystem::path> path)
{
    m_path = std::move(path);
}


void Log::write(
    const std::string &message,
    const Severity severity,
    const std::string &file,
    const int line,
    const std::string &functionName)
{
    std::ignore = file;
    std::ignore = line;
    std::ignore = functionName;

    std::unique_lock<std::mutex> lock(m_mutex);

    std::ostream &stream = severity <= Log::Severity::Warning ? std::cout : std::cerr;
    ::write(stream, message, severity, file, line, functionName);

    if (m_path.has_value())
    {
        if (m_filename.empty())
        {
            m_filename = makeLogFilename(m_path.value());
        }
        checkLogSize(m_filename);
        std::ofstream fstream(m_filename, std::ios::app);
        ::write(fstream, message, severity, file, line, functionName);
        fstream.close();
    }
}


Log::~Log()
{
    // write("Exiting program.", Log::Severity::Info);
}
