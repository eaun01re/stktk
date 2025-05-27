#include <game/game.h>
#include <game/log.h>
#include <game/version/version.h>


#ifdef BUILD_WITH_WINDOWSCRASHDUMP
#include <Windows.h>
#include <DbgHelp.h>

#include <game/clock.h>

LONG WINAPI customUnhandledExceptionFilter(PEXCEPTION_POINTERS pExInfo)
{
    const std::string fileName =
        "crashdump_" + formatTimeEscaped()
        + "_" + ProjectVersion + ".dmp";
    LOG_FATAL(fileName)

    HANDLE hFile = CreateFile(
        fileName.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (nullptr == hFile || INVALID_HANDLE_VALUE == hFile)
    {
        LOG_FATAL("Unhandled Exception. hFile = " << unsigned(hFile) << '.');
        return EXCEPTION_EXECUTE_HANDLER;
    }

    MINIDUMP_EXCEPTION_INFORMATION eInfo;
    eInfo.ThreadId = GetCurrentThreadId();
    eInfo.ExceptionPointers = pExInfo;
    eInfo.ClientPointers = FALSE;

    const MINIDUMP_TYPE minidumpType = _MINIDUMP_TYPE::MiniDumpWithFullMemory;
    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        minidumpType,
        &eInfo,
        nullptr,
        nullptr);
    CloseHandle(hFile);

    LOG_FATAL("Unhandled Exception. See " << fileName);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif // BUILD_WITH_WINDOWSCRASHDUMP


std::optional<unsigned int> positionNumber(const char *string)
{
    std::optional<unsigned int> position;
    std::size_t pos{};
    try
    {
        position = std::stoi(string, &pos);
    }
    catch (const std::exception &ex)
    {
        LOG_ERROR(
            "Unable to cast position with number \"" << string
            << "\". Error: " << ex.what() << ".");
    }
    return position;
}


int main(int argc, char *argv[])
{
    // Включение записи лога в файл.
    Log::instance().setPath(".");

    LOG_INFO("--- Starting " << ProjectName << ' ' << ProjectVersion << " ---");

#ifdef BUILD_WITH_WINDOWSCRASHDUMP
    const LPTOP_LEVEL_EXCEPTION_FILTER hOldFilter =
        SetUnhandledExceptionFilter(customUnhandledExceptionFilter);
    std::ignore = hOldFilter;
#endif // BUILD_WITH_WINDOWSCRASHDUMP

    Game game;
    if (game.init())
    {
        LOG_INFO("All resources loaded.");
    }
    else
    {
        LOG_ERROR("Failed to initialize application.");
        return 1;
    }
    const std::optional<unsigned int> position = argc > 1 ? positionNumber(argv[1]) : std::nullopt;
    game.start(position);
    while (!game.isDone())
    {
        game.restartClock();
        game.update();
        game.render();
    }

    LOG_INFO("--- Exiting ---");

    return 0;
}


#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrev,
    LPSTR lpszCmdLine,
    int nCmdShow)
{
    std::ignore = hInst;
    std::ignore = hPrev;
    std::ignore = lpszCmdLine;
    std::ignore = nCmdShow;
    return main(__argc, __argv);
}
#endif
