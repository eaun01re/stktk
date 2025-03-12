#include "box.h"

#include <cmath>

#include "math.h"
#include "objects/object.h"


Box::Box(Id id)
    : m_id(id)
{
}


Box::Box(const Box &box)
    : m_id(box.id())
{
    init(box.m_texture);
    setPosition(box.position());
    setStyle(box.style());
}


void Box::init(const sf::Texture &texture)
{
    Object::init(texture);
}


Box::Id Box::id() const noexcept
{
    return m_id;
}


unsigned int Box::style() const noexcept
{
    return m_style;
}


void Box::setStyle(unsigned int style)
{
    m_style = style;
    m_sprite.setTextureRect(mirrorVertical(sf::IntRect(m_style * BOX_SIZE, 0, BOX_SIZE, BOX_SIZE)));
}


void Box::move(Direction direction)
{
    m_direction = direction;
    switch (direction)
    {
    case Direction::None:
        break;
    case Direction::Left:
        m_speed = sf::Vector2f(-SPEED, 0);
        m_movementLength = sf::Vector2f(-BOX_SIZE, 0);
        break;
    case Direction::Right:
        m_speed = sf::Vector2f(SPEED, 0);
        m_movementLength = sf::Vector2f(BOX_SIZE, 0);
        break;
    case Direction::Down:
        m_speed = sf::Vector2f(0, -SPEED);
        m_movementLength = sf::Vector2f(0, -5 * BOX_SIZE);
        break;
    }
}


Box::Direction Box::direction() const noexcept
{
    return m_direction;
}


void Box::update(const Duration &elapsed)
{
    if (!isTolerant(m_speed))
    {
        return;
    }

    const double elapsedSeconds = elapsed.count();
    const sf::Vector2f offset = multiply(m_speed, elapsedSeconds);
    m_sprite.move(offset.x, offset.y);

    if ((m_movementLength.x != 0 &&
         std::isless(m_movementLength.x, 0.0f) == std::isless(m_movementLength.x - offset.x, 0.0f)) ||
        (m_movementLength.y != 0 &&
         std::isless(m_movementLength.y, 0.0f) == std::isless(m_movementLength.y - offset.y, 0.0f)))
    {
        m_movementLength.x -= offset.x;
        m_movementLength.y -= offset.y;
    }
    else
    {
        normalizePosition(true, true);
        m_speed = sf::Vector2f();
        m_movementLength = sf::Vector2f();
    }
}
