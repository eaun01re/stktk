#include "game.h"

#include <iostream>

#include "log.h"
#include "version/version.h"


namespace
{

const std::string RESOURCES_DIR("resources/");
const std::string TEXTURE_BOX("box.png");
const std::string TEXTURE_PLAYER("player.png");
const std::string TEXTURE_BACKGROUND("background.png");

}


bool loadTexture(
    const std::string &file,
    sf::Texture &texture,
    std::string* errorDescription)
{
    const bool result = texture.loadFromFile(RESOURCES_DIR + file);
    if (!result)
    {
        if (errorDescription != nullptr)
        {
            *errorDescription = "Could not load resource '" + file + '\'';
        }
        return result;
    }
    return result;
}


Game::Game()
    : m_window(
        ProjectName,
        std::bind(&Game::windowResized, this, std::placeholders::_1),
        std::bind(&Game::keyPressed, this, std::placeholders::_1))
{
    restartClock();
}


bool Game::init(std::string* errorDescription)
{
    // Загрузка текстур и звуков.
    sf::Texture textureBox;
    sf::Texture texturePlayer;
    sf::Texture textureBackground;
    const bool result =
        loadTexture(TEXTURE_BOX, textureBox, errorDescription) &&
        loadTexture(TEXTURE_PLAYER, texturePlayer, errorDescription) &&
        loadTexture(TEXTURE_BACKGROUND, textureBackground, errorDescription);
    if (!result)
    {
        return result;
    }

    m_world.init(
        textureBackground,
        texturePlayer,
        textureBox);
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
    const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    m_elapsed = std::chrono::duration_cast<Duration>(now - m_time);
    m_time = std::chrono::high_resolution_clock::now();
#else
    m_elapsed = fromMicroseconds(1e6 / FPS_LIMIT);
#endif
}


void Game::reset(const std::optional<unsigned int> &position)
{
    m_world.generateStartPosition(position);
}


void Game::windowResized(const sf::Vector2u &size)
{
    LOG_DEBUG("Window is resized to " << size.x << 'x' << size.y << '.');
    m_window.resize(size);
}


void Game::keyPressed(const sf::Event::KeyEvent &key)
{
    switch (key.code)
    {
    case sf::Keyboard::Key::Left:
    case sf::Keyboard::Key::A:
        m_world.movePlayer(Player::Direction::Left);
        break;
    case sf::Keyboard::Key::Right:
    case sf::Keyboard::Key::D:
        m_world.movePlayer(Player::Direction::Right);
        break;
    case sf::Keyboard::Key::Up:
    case sf::Keyboard::Key::W:
        m_world.movePlayer(Player::Direction::Up);
        break;
    case sf::Keyboard::Key::Q:
        m_world.movePlayer(Player::Direction::UpLeft);
        break;
    case sf::Keyboard::Key::E:
        m_world.movePlayer(Player::Direction::UpRight);
        break;
    default:
        break;
    }
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
