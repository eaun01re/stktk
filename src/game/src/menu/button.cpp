#include <game/menu/button.h>

#include <cmath>

#include <game/consts.h>


namespace
{

const sf::Color COLOR = sf::Color::Black;

/// Возвращает список точек, составляющих текстуру шахматной доски.
/// \code
/// o o o o
///  o o o
/// o o o o
/// \endcode
std::vector<sf::RectangleShape> makeDisabledButton(const sf::Vector2f &position)
{
    std::vector<sf::RectangleShape> result;
    result.reserve((BUTTON_SIZE.x + BUTTON_SIZE.y) / 2);
    for (unsigned int y = 0; y < BUTTON_SIZE.y; ++y)
    {
        for (unsigned int x = y % 2; x < BUTTON_SIZE.x; x += 2)
        {
            sf::RectangleShape dot(sf::Vector2f(1, 1));
            dot.setFillColor(COLOR);
            dot.setPosition(position.x + x, position.y + y);
            result.push_back(dot);
        }
    }
    return result;
}


std::vector<sf::RectangleShape> makeEnabledButton(const sf::Vector2f &position)
{
    std::vector<sf::RectangleShape> result;
    sf::RectangleShape rectangle(sf::Vector2f(BUTTON_SIZE.x, BUTTON_SIZE.y));
    rectangle.setFillColor(COLOR);
    rectangle.setPosition(position);
    result.push_back(rectangle);
    return result;
}

}


Button::Button(const std::u32string &caption)
{
    m_text.setBold(true);
    m_text.setAlign(Text::Align::Center);
    m_text.setText(caption);
    m_text.setColor(BACKGROUND_COLOR);
    update();
}


void Button::draw(sf::RenderTarget &target, sf::RenderStates) const
{
    for (const auto &dot : m_background)
    {
        target.draw(dot);
    }

    target.draw(m_text);
}


const sf::Vector2f &Button::position() const
{
    return m_position;
}


void Button::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    update();
}


void Button::setCaption(const std::u32string &caption)
{
    m_text.setText(caption);
    update();
}


bool Button::enabled() const noexcept
{
    return !m_text.text().empty();
}


void Button::update()
{
    m_background = enabled()
        ? makeEnabledButton(m_position)
        : makeDisabledButton(m_position);
    m_text.setPosition(sf::Vector2f(
        std::ceil(m_position.x + BUTTON_SIZE.x / 2.0f),
        m_position.y));
}
