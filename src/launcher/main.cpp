#include <game/game.h>
#include <game/log.h>
#include <game/version/version.h>


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
    // Если необходимо записывать лог в файл.
    // Log::instance().setPath(".");
    LOG_INFO("--- Starting " << ProjectName << ' ' << ProjectVersion << " ---");

    Game game;
    if (!game.init())
    {
        LOG_ERROR("Failed to initialize application.");
        return 1;
    }
    const std::optional<unsigned int> position = argc > 1 ? positionNumber(argv[1]) : std::nullopt;
    game.start(1, position);
    while (!game.window().isDone())
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
    return main(__argc, __argv);
}
#endif
