#include "game.h"

#include <iostream>

#include "log.h"
#include "resource_loader.h"
#include "version/version.h"


namespace
{

Player::Direction directionByKey(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::Key::Left:
    case sf::Keyboard::Key::A:
        return Player::Direction::Left;
    case sf::Keyboard::Key::Right:
    case sf::Keyboard::Key::D:
        return Player::Direction::Right;
    case sf::Keyboard::Key::Up:
    case sf::Keyboard::Key::W:
        return Player::Direction::Up;
    case sf::Keyboard::Key::Q:
        return Player::Direction::UpLeft;
    case sf::Keyboard::Key::E:
        return Player::Direction::UpRight;
    default:
        return Player::Direction::None;
    }
}

}


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
    m_world.init();
    return result;
}


// sf::Time Game::elapsed() const
// {
//     return m_elapsed;
// }


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
    m_world.reset(cranesQuantity, position);
}


void Game::windowResized(const sf::Vector2u &size)
{
    LOG_DEBUG("Window is resized to " << size.x << 'x' << size.y << '.');
    m_window.resize(size);
}


void Game::keyPressed(const sf::Event::KeyEvent &key)
{
    const Player::Direction requestedDirection = directionByKey(key.code);
    if (requestedDirection == Player::Direction::None)
    {
        return;
    }
    m_world.requestMovePlayer(requestedDirection);
}


void Game::keyReleased(const sf::Event::KeyEvent &key)
{
    const Player::Direction requestedDirection = directionByKey(key.code);
    if (requestedDirection == Player::Direction::None)
    {
        return;
    }
    m_world.requestStopPlayer();
}


const Window& Game::window() const
{
    return m_window;
}


//void Game::handleInput()
//{
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
//        //&& m_snake.GetPhysicalDirection() != Direction::Down
//        )
//    {
//        std::cout << 1 << std::endl;
////        m_snake.SetDirection(Direction::Up);
//    }
//    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
//        //&& m_snake.GetPhysicalDirection() != Direction::Up
//               )
//    {
//        std::cout << 2 << std::endl;
////        m_snake.SetDirection(Direction::Down);
//    }
//    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
//        //&& m_snake.GetPhysicalDirection() != Direction::Right
//               )
//    {
//        std::cout << 3 << std::endl;
////        m_snake.SetDirection(Direction::Left);
//    }
//    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
//        //&& m_snake.GetPhysicalDirection() != Direction::Left
//               )
//    {
//        std::cout << 4 << std::endl;
////        m_snake.SetDirection(Direction::Right);
//    }
//}


void Game::update()
{
    m_world.update(m_elapsed);
    m_window.update();
}


void Game::render()
{
    // Clear.
    m_window.beginDraw();

    m_world.render(m_window.renderWindow());

    // Display.
    m_window.endDraw();
}
