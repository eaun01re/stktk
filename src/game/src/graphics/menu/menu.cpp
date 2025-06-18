#include <game/graphics/menu/menu.h>

#include <boost/signals2.hpp>

#include <game/log.h>


namespace
{

constexpr sf::Keyboard::Key KEY_LEFT = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key KEY_RIGHT = sf::Keyboard::Key::Right;
constexpr sf::Keyboard::Key KEY_BACK = sf::Keyboard::Key::Escape;

/*!
 * Возвращает список графических примитивов, составляющих рамку меню.
 * \return Прямоугольники, составляющие рамку.
 */
std::list<sf::RectangleShape> makeFrame()
{
    sf::RectangleShape background(sf::Vector2f(
        FRAME_SIZE.x - 3,
        FRAME_SIZE.y - 3));
    background.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 1,
        FRAME_POSITION.y + 1));
    background.setFillColor(BACKGROUND_COLOR);

    sf::RectangleShape topLine(sf::Vector2f(FRAME_SIZE.x - 5, 1));
    topLine.setPosition(sf::Vector2f(FRAME_POSITION.x + 2, FRAME_POSITION.y));
    topLine.setFillColor(TEXT_COLOR);

    sf::RectangleShape rightLine(sf::Vector2f(1, FRAME_SIZE.y - 4));
    rightLine.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 2,
        FRAME_POSITION.y + 2));
    rightLine.setFillColor(TEXT_COLOR);
    sf::RectangleShape rightLine2(sf::Vector2f(1, FRAME_SIZE.y - 6));
    rightLine2.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 1,
        FRAME_POSITION.y + 3));
    rightLine2.setFillColor(TEXT_COLOR);

    sf::RectangleShape bottomLine(sf::Vector2f(FRAME_SIZE.x - 4, 1));
    bottomLine.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 2,
        FRAME_POSITION.y + FRAME_SIZE.y - 2));
    bottomLine.setFillColor(TEXT_COLOR);
    sf::RectangleShape bottomLine2(sf::Vector2f(FRAME_SIZE.x - 6, 1));
    bottomLine2.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 3,
        FRAME_POSITION.y + FRAME_SIZE.y - 1));
    bottomLine2.setFillColor(TEXT_COLOR);

    sf::RectangleShape leftLine(sf::Vector2f(1, FRAME_SIZE.y - 5));
    leftLine.setPosition(sf::Vector2f(FRAME_POSITION.x, FRAME_POSITION.y + 2));
    leftLine.setFillColor(TEXT_COLOR);

    sf::RectangleShape topLeftDot(sf::Vector2f(1, 1));
    topLeftDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 1,
        FRAME_POSITION.y + 1));
    topLeftDot.setFillColor(TEXT_COLOR);
    sf::RectangleShape topRightDot(sf::Vector2f(1, 1));
    topRightDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 3,
        FRAME_POSITION.y + 1));
    topRightDot.setFillColor(TEXT_COLOR);
    sf::RectangleShape bottomRightDot(sf::Vector2f(1, 1));
    bottomRightDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 3,
        FRAME_POSITION.y + FRAME_SIZE.y - 3));
    bottomRightDot.setFillColor(TEXT_COLOR);
    sf::RectangleShape bottomLeftDot(sf::Vector2f(1, 1));
    bottomLeftDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 1,
        FRAME_POSITION.y + FRAME_SIZE.y - 3));
    bottomLeftDot.setFillColor(TEXT_COLOR);

    const std::list<sf::RectangleShape> frame =
    {
        background,
        topLine,
        rightLine,
        rightLine2,
        bottomLine,
        bottomLine2,
        leftLine,
        topLeftDot,
        topRightDot,
        bottomRightDot,
        bottomLeftDot,
    };
    return frame;
}

}


Menu::Menu()
{
    const unsigned int buttonTop = SCREEN_SIZE.y - BUTTON_SIZE.y;
    m_buttonLeft.setPosition(
        sf::Vector2f(0, buttonTop));
    m_buttonRight.setPosition(
        sf::Vector2f(SCREEN_SIZE.x - BUTTON_SIZE.x, buttonTop));
}


bool Menu::handleKeyPressed(const sf::Keyboard::Key key)
{
    switch (key)
    {
    case KEY_LEFT:
        onLeftActivated();
        return true;
    case KEY_RIGHT:
        onRightActivated();
        return true;
    case KEY_BACK:
        close();
        return true;
    default:
        break;
    }

    return false;
}


void Menu::draw(sf::RenderTarget &target, sf::RenderStates) const
{
    target.draw(m_buttonLeft);
    target.draw(m_buttonRight);

    for (const sf::RectangleShape &rectangle : m_frame)
    {
        target.draw(rectangle);
    }
}


boost::signals2::connection Menu::connectClose(const Slot &slot)
{
    return m_signalClose.connect(slot);
}


void Menu::makeFrame()
{
    m_frame = ::makeFrame();
}


void Menu::onLeftActivated()
{
    LOG_DEBUG("Left action requested.");
    m_signalLeft();
}


void Menu::onRightActivated()
{
    LOG_DEBUG("Right action requested.");
    m_signalRight();
}


void Menu::close()
{
    LOG_DEBUG("Close action requested.");
    m_signalClose();
}


boost::signals2::connection Menu::connectLeft(const Slot &slot)
{
    m_connectionLeft.disconnect();
    m_connectionLeft = m_signalLeft.connect(slot);
    return m_connectionLeft;
}


boost::signals2::connection Menu::connectRight(const Slot &slot)
{
    m_connectionRight.disconnect();
    m_connectionRight = m_signalRight.connect(slot);
    return m_connectionRight;
}
