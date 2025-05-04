#pragma once


#include <memory>

#include "window.h"
#include "start.h"


class Game final
{
public:
    explicit Game();

    bool init();
    void start(
        const std::uint8_t cranesQuantity = 1,
        const std::optional<unsigned int> &position = std::nullopt);
    void handleInput();
    void update();
    void render();

    const Window& window() const;

    void restartClock();

private:
    std::unique_ptr<Start> makeStartScreen(const std::uint8_t cranesQuantity);
    void onWindowResized(const sf::Vector2u &size);
    void onKeyPressed(const sf::Event::KeyEvent &key);
    void onKeyReleased(const sf::Event::KeyEvent &key);
    void onStartScreenClosed(const std::optional<Start::Config> &config);
    void renderClippingMask(sf::RenderTarget &target);
    void exit();

private:
    Window m_window;

    std::unique_ptr<Screen> m_screen;

    /// Маска из двух прямоугольников по бокам, прикрывающая объекты,
    /// выходящие за пределывиртуального экрана.
    /// TODO: использовать готовое решение в SFML 3:
    /// https://github.com/SFML/SFML/pull/1451
    sf::RectangleShape m_maskLeft;
    sf::RectangleShape m_maskRight;

#if defined(USE_SFML_CLOCK)
    sf::Clock m_clock;
#elif defined(USE_STD_CLOCK)
    TimePoint m_time;
#endif
    Duration m_elapsed;

    std::optional<unsigned int> m_initialPosition;
};
