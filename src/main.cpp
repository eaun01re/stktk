#include "game.h"
#include "log.h"
#include "version/version.h"

// Game example.
// https://github.com/sschellhoff/SFMLGameDevelopmentByExample/tree/master/chapter_03

// [ C++ & SFML - Simple 2D Games ] - Easy Platformer pt.11 - New Input System + Add & Remove tiles!
// https://www.youtube.com/watch?v=g6qxn2T0guo&list=PL6xSOsbVA1eb_QqMTTcql_3PdOiE928up


/// TODO:
/// Ящик можно толкать во время его падения, если он ниже пояса.
/// Если игрок пытается перейти в колонку, на которую падает ящик, то
/// * ящик не блокирует движение, если находится выше головы
/// * блокирует движение, если находится ниже головы, но выше пояса.
/// * сдвигается вместе с игроком, если находится ниже пояса, но выше уровня земли, на которой стоит игрок.
/// * не блокирует движение, если находится ниже уровня земли, на которой стоит игрок
///   и позволяет на него взойти, если его верхний край выше уровня земли. По нему можно перейти "пропасть".
/// при сталкивании ящика с одного яруса на более нижний ящик падает не сразу когда кончается верхний ярус,
/// а пролетает на одну клетку дальше! как только ящик после толчка оказывается в воздухе его можно успеть толкнуть
/// В момент толчка ящик ненадолго зависает в воздухе, после чего продолжает падать с замедленной скоростью
/// https://www.youtube.com/watch?v=6PaLFAZN518&t=75
/// Можно прыгать по диагонали, если ящик, на который в итоге встанет игрок,
/// еще падает, но успеет остановиться к моменту приземления игрока.
/// Кран может проехать, не скинув ящика.
/// Можно сбивать ящик прыжком по диагонали.


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
    game.reset(3, position);
    while (!game.window().isDone())
    {
//        game.handleInput();
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
