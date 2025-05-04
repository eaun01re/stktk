#include "game.h"

#include <iostream>

#include "log.h"
#include "world.h"
#include "resource_loader.h"
#include "version/version.h"


Game::Game()
    : m_window(
        ProjectName,
        std::bind(&Game::onWindowResized, this, std::placeholders::_1),
        std::bind(&Game::onKeyPressed, this, std::placeholders::_1),
        std::bind(&Game::onKeyReleased, this, std::placeholders::_1))
{
    restartClock();
    m_maskLeft.setFillColor(sf::Color::Black);
    m_maskRight.setFillColor(sf::Color::Black);
}


bool Game::init()
{
    // Загрузка текстур и звуков.
    return ResourceLoader::instance().load();
}


void Game::restartClock()
{
#if defined(USE_SFML_CLOCK)
    // m_elapsed = fromMicroseconds(m_clock.restart().asMicroseconds());
    // Фиксированный интервал, для отладки.
    m_elapsed = fromMicroseconds(1e6 / FPS_LIMIT);
#elif defined(USE_STD_CLOCK)
    const TimePoint now = Clock::now();
    m_elapsed = std::chrono::duration_cast<Duration>(now - m_time);
    m_time = Clock::now();
#elif defined(USE_FIXED_CLOCK)
    m_elapsed = fromMicroseconds(1e6 / FPS_LIMIT);
#endif
}


std::unique_ptr<Start> Game::makeStartScreen(const std::uint8_t cranesQuantity)
{
    std::unique_ptr<Start> screen = std::make_unique<Start>(
        std::bind(&Game::onStartScreenClosed, this, std::placeholders::_1),
        cranesQuantity);
    return screen;
}


void Game::start(
    const std::uint8_t cranesQuantity,
    const std::optional<unsigned int> &position)
{
    m_initialPosition = position;
    m_screen = makeStartScreen(cranesQuantity);
}


void Game::onWindowResized(const sf::Vector2u &size)
{
    LOG_DEBUG("Window is resized to " << size.x << 'x' << size.y << '.');
    m_window.resize(size);

    // Обновление обрезающей области.
    const sf::Vector2f &viewSize = m_window.viewSize();
    const sf::Vector2f maskSize((viewSize.x - SCREEN_SIZE.x) / 2, viewSize.y);
    m_maskLeft.setSize(maskSize);
    m_maskRight.setSize(maskSize);
    m_maskLeft.setPosition(-maskSize.x, 0);
    m_maskRight.setPosition(float(SCREEN_SIZE.x), 0);
}


void Game::onKeyPressed(const sf::Event::KeyEvent &key)
{
    m_screen->handleKeyPressed(key.code);
}


void Game::onKeyReleased(const sf::Event::KeyEvent &key)
{
    m_screen->handleKeyReleased(key.code);
}


void Game::onStartScreenClosed(const std::optional<Start::Config> &config)
{
    if (!config.has_value())
    {
        exit();
        return;
    }

    std::unique_ptr<World> worldScreen = std::make_unique<World>();
    worldScreen->start(config.value().cranesQuantity, m_initialPosition);
    m_screen = std::move(worldScreen);
}


const Window& Game::window() const
{
    return m_window;
}


void Game::update()
{
    m_screen->update(m_elapsed);
    m_window.update();
}


void Game::render()
{
    // Clear.
    m_window.beginDraw();

    sf::RenderWindow& target = m_window.renderWindow();
    m_screen->render(target);
    renderClippingMask(target);

    // Display.
    m_window.endDraw();
}


void Game::renderClippingMask(sf::RenderTarget &target)
{
    target.draw(m_maskLeft);
    target.draw(m_maskRight);
}


void Game::exit()
{
    m_window.close();
}
