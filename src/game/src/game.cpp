#include <game/game.h>

#include <iostream>

#include <game/config.h>
#include <game/log.h>
#include <game/world.h>
#include <game/resource_loader.h>
#include <game/version/version.h>


Game::Game()
    : m_window(
        ProjectName
#ifndef NDEBUG
        + " " + ProjectVersion
#endif
        ,
        std::bind(&Game::onWindowResized, this, std::placeholders::_1),
        std::bind(&Game::onKeyPressed, this, std::placeholders::_1),
        std::bind(&Game::onKeyReleased, this, std::placeholders::_1))
{
    setup();
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


void Game::setup()
{
    restartClock();

    m_maskLeft.setFillColor(sf::Color::Black);
    m_maskRight.setFillColor(sf::Color::Black);
}


std::shared_ptr<MenuStart> Game::makeStartScreen()
{
    std::shared_ptr<MenuStart> screen = std::make_shared<MenuStart>(
        std::bind(&Game::onStartScreenClosed, this, std::placeholders::_1));
    return screen;
}


void Game::start(const std::optional<unsigned int> &position)
{
    m_initialPosition = position;
    m_screen = makeStartScreen();
}


void Game::onWindowResized(const sf::Vector2u &size)
{
    LOG_DEBUG("Window is resized to " << size.x << 'x' << size.y << '.');
    m_window.resize(size);

    // Обновление обрезающей области.
    // TODO: Обрезать и по вертикали?
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


void Game::onStartScreenClosed(bool start)
{
    if (!start)
    {
        exit();
        return;
    }

    std::shared_ptr<World> worldScreen = std::make_shared<World>();
    worldScreen->start(m_initialPosition);
    m_screen = worldScreen;

#ifndef NDEBUG
    m_debug = ScreenDebug(worldScreen);
#endif

    m_window.enableDebugView(m_debug.has_value());
    worldScreen->setDebugMode(m_debug.has_value());
}


const Window& Game::window() const
{
    return m_window;
}


void Game::update()
{
    m_screen->update(m_elapsed);
    if (m_debug.has_value())
    {
        m_debug.value().update(m_elapsed);
    }
    m_window.update();
}


void Game::render()
{
    // Clear.
    m_window.beginDraw();

    sf::RenderWindow& target = m_window.gameWindow();
    target.draw(*m_screen);
    renderClippingMask(target);

    if (m_debug.has_value())
    {
        sf::RenderWindow& targetDebug = m_window.debugWindow();
        targetDebug.draw(m_debug.value());
    }

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
