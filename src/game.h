#pragma once


#include "window.h"
#include "world.h"


class Game final
{
public:
    explicit Game();

    bool init(std::string* errorDescription = nullptr);
    void reset(const std::optional<unsigned int> &position = std::nullopt);
    void handleInput();
    void update();
    void render();

    const Window& window() const;

    // sf::Time elapsed() const;
    void restartClock();

private:
    void windowResized(const sf::Vector2u &size);
    void keyPressed(const sf::Event::KeyEvent &key);

private:
    Window m_window;

    World m_world;

#if defined(USE_SFML_CLOCK)
    sf::Clock m_clock;
#elif defined(USE_STD_CLOCK)
    std::chrono::high_resolution_clock::time_point m_time;
#endif
    Duration m_elapsed;

    mutable std::mt19937 m_randomEngine;
    mutable std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
};
