// Game example.
// https://github.com/sschellhoff/SFMLGameDevelopmentByExample/tree/master/chapter_03

// [ C++ & SFML - Simple 2D Games ] - Easy Platformer pt.11 - New Input System + Add & Remove tiles!
// https://www.youtube.com/watch?v=g6qxn2T0guo&list=PL6xSOsbVA1eb_QqMTTcql_3PdOiE928up


// TODO: Падающий ящик не блокирует движение, если находится ниже уровня земли,
// на которой стоит игрок и позволяет на него взойти, если его верхний край
// выше уровня земли. По нему можно перейти "пропасть".
// При сталкивании ящика с одного яруса на более нижний ящик падает не сразу когда кончается верхний ярус,
// а пролетает на одну клетку дальше! как только ящик после толчка оказывается в воздухе его можно успеть толкнуть
// В момент толчка ящик ненадолго зависает в воздухе, после чего продолжает падать с замедленной скоростью
// https://www.youtube.com/watch?v=6PaLFAZN518&t=75


#pragma once


#include <memory>
#include <optional>

#include <game/menu_screen.h>
#include <game/window.h>
#include <game/screen_debug.h>


class Game final
{
public:
    explicit Game();

    bool init();
    void start(const std::optional<unsigned int> &position = std::nullopt);
    void handleInput();
    void update();
    void render();
    bool isDone() const noexcept;
    void restartClock();

private:
    void setup();
    std::shared_ptr<MenuScreen> makeStartScreen();
    std::shared_ptr<World> makeWorldScreen();
    void onWindowResized(const sf::Vector2u &size);
    void onKeyPressed(const sf::Event::KeyEvent &key);
    void onKeyReleased(const sf::Event::KeyEvent &key);
    void onMenuScreenClosed(bool startGame);
    void onWorldScreenClosed();
    void exit();

private:
    Window m_window;

    std::shared_ptr<Screen> m_screen;
    std::optional<ScreenDebug> m_debug;

#if defined(USE_SFML_CLOCK)
    sf::Clock m_clock;
#elif defined(USE_STD_CLOCK)
    TimePoint m_time;
#endif
    Duration m_elapsed;

    std::optional<unsigned int> m_initialPosition;
};
