#include "game.h"

#include <iostream>

#include "log.h"
#include "resource_loader.h"
#include "version/version.h"


Game::Game()
    : m_window(
        ProjectName,
        std::bind(&Game::windowResized, this, std::placeholders::_1),
        std::bind(&Game::keyPressed, this, std::placeholders::_1),
        std::bind(&Game::keyReleased, this, std::placeholders::_1))
{
    restartClock();
}


bool Game::init()
{
    // Загрузка текстур и звуков.
    const bool result = ResourceLoader::instance().load();
    if (!result)
    {
        return false;
    }

    m_world.reset(new World());
    m_world->init();
    return result;
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


void Game::reset(
    const std::uint8_t cranesQuantity,
    const std::optional<unsigned int> &position)
{
    m_world->start(cranesQuantity, position);
}


void Game::windowResized(const sf::Vector2u &size)
{
    LOG_DEBUG("Window is resized to " << size.x << 'x' << size.y << '.');
    m_window.resize(size);
}


void Game::keyPressed(const sf::Event::KeyEvent &key)
{
    m_world->handleKeyPressed(key.code);
}


void Game::keyReleased(const sf::Event::KeyEvent &key)
{
    m_world->handleKeyReleased(key.code);
}


const Window& Game::window() const
{
    return m_window;
}


void Game::update()
{
    m_world->update(m_elapsed);
    m_window.update();
}


void Game::render()
{
    // Clear.
    m_window.beginDraw();

    m_world->render(m_window.renderWindow());

    // Display.
    m_window.endDraw();
}
