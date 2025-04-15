#pragma once


#include "window.h"
#include "world.h"


class Game final
{
public:
    explicit Game();

    bool init();
    void reset(
        const std::uint8_t cranesQuantity = 1,
        const std::optional<unsigned int> &position = std::nullopt);
    void handleInput();
    void update();
    void render();

    const Window& window() const;

    void restartClock();

private:
    void windowResized(const sf::Vector2u &size);
    void keyPressed(const sf::Event::KeyEvent &key);
    void keyReleased(const sf::Event::KeyEvent &key);
    void renderClippingMask(sf::RenderTarget &target);

private:
    Window m_window;

    std::unique_ptr<World> m_world;

    /// Маска из двух прямоугольников по бокам, прикрывающая объекты,
    /// выходящие за пределывиртуального экрана.
    /// FIXME: использовать готовое решение в SFML 3:
    /// https://github.com/SFML/SFML/pull/1451
    sf::RectangleShape m_maskLeft;
    sf::RectangleShape m_maskRight;

#if defined(USE_SFML_CLOCK)
    sf::Clock m_clock;
#elif defined(USE_STD_CLOCK)
    TimePoint m_time;
#endif
    Duration m_elapsed;

    mutable std::mt19937 m_randomEngine;
    mutable std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
};
