#include <game/game.h>

#include <iostream>
#include <boost/bind/bind.hpp>

#include <game/config.h>
#include <game/log.h>
#include <game/world.h>
#include <game/resource_loader.h>
#include <game/serializer.h>
#include <game/version/version.h>


Game::Game()
    : m_window(
        ProjectName + " " + ProjectVersion,
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
    if (!readConfig())
    {
        writeConfig();
    }

    restartClock();
}


std::shared_ptr<MenuScreen> Game::makeStartScreen()
{
    std::shared_ptr<MenuScreen> menuScreen = std::make_shared<MenuScreen>();
    menuScreen->connectClose(
        boost::bind(&Game::onMenuScreenClosed, this, boost::placeholders::_1));
    return menuScreen;
}


std::shared_ptr<World> Game::makeWorldScreen()
{
    std::shared_ptr<World> worldScreen = std::make_shared<World>();
    worldScreen->connectClose(boost::bind(&Game::onWorldScreenClosed, this));
    worldScreen->start(m_initialPosition);
    return worldScreen;
}


void Game::start(const std::optional<unsigned int> &position)
{
    m_initialPosition = position;
    m_screen = makeStartScreen();
    m_debug = std::nullopt;
}


void Game::onWindowResized(const sf::Vector2u &size)
{
    LOG_DEBUG("Window is resized to " << size.x << 'x' << size.y << '.');
    m_window.resize(size);
}


void Game::onKeyPressed(const sf::Event::KeyEvent &key)
{
    m_screen->handleKeyPressed(key.code);
}


void Game::onKeyReleased(const sf::Event::KeyEvent &key)
{
    m_screen->handleKeyReleased(key.code);
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
    m_window.beginDraw();

    sf::RenderWindow &target = m_window.gameWindow();
    target.draw(*m_screen);
    m_window.renderClippingMask(target);

    if (m_debug.has_value())
    {
        sf::RenderWindow &targetDebug = m_window.debugWindow();
        targetDebug.draw(m_debug.value());
    }

    m_window.endDraw();
}


bool Game::isDone() const noexcept
{
    return m_window.isDone();
}


void Game::onMenuScreenClosed(bool startGame)
{
    if (!startGame)
    {
        exit();
        return;
    }

    std::shared_ptr<World> worldScreen = makeWorldScreen();
    m_screen = worldScreen;

#ifndef NDEBUG
    m_debug = ScreenDebug(worldScreen);
#endif

    m_window.enableDebugView(m_debug.has_value());
    worldScreen->setDebugMode(m_debug.has_value());
}


void Game::onWorldScreenClosed()
{
    start();
    writeConfig();
}


void Game::exit()
{
    m_window.close();
}
