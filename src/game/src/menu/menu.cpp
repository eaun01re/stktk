#include <game/menu/menu.h>

#include <game/consts.h>


namespace
{

constexpr sf::Keyboard::Key KEY_LEFT = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key KEY_RIGHT = sf::Keyboard::Key::Right;
constexpr sf::Keyboard::Key KEY_BACK = sf::Keyboard::Key::Escape;

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
    case KEY_BACK:
        onCloseActivated();
        return true;
    case KEY_LEFT:
        if (m_buttonLeft.enabled())
        {
            onLeftActivated();
            return true;
        }
        break;
    case KEY_RIGHT:
        if (m_buttonLeft.enabled())
        {
            onRightActivated();
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}


void Menu::draw(sf::RenderTarget &target, sf::RenderStates) const
{
    target.draw(m_buttonLeft);
    target.draw(m_buttonRight);
}


void Menu::onCloseActivated()
{
}


void Menu::onLeftActivated()
{
}


void Menu::onRightActivated()
{
}
