#include "object.h"

#include <cmath>

#include "math.h"


namespace
{

constexpr float COORDINATE_TOLERANCE = 0.01;

}


Object::Object()
{
    m_sprite.setPosition(0, 0);
}


void Object::init(const sf::Texture &texture)
{
    m_texture = texture;
    m_sprite.setTexture(m_texture);
}


void Object::setPosition(const sf::Vector2f &position)
{
    m_sprite.setPosition(position);
}


const sf::Vector2f& Object::position() const noexcept
{
    return m_sprite.getPosition();
}


const sf::Vector2f& Object::speed() const noexcept
{
    return m_speed;
}


bool Object::isFalling() const noexcept
{
    return m_speed.x == 0 && m_speed.y < 0;
}


bool Object::isMoving() const
{
    return isTolerant(m_speed);
}


void Object::stopFalling()
{
    m_speed.y = 0;
    m_movementLength.y = 0;
    normalizePosition(false, true);
}


std::optional<Object::Coordinate> Object::column() const
{
    const bool hasCertainColumn = std::abs(normalizedPosition(position(), true, false).x - position().x) < COORDINATE_TOLERANCE;
    const std::optional<unsigned int> result = hasCertainColumn
        ? std::optional<unsigned int>(std::round(position().x / BOX_SIZE))
        : std::nullopt;
    return result;
}


std::optional<Object::Coordinate> Object::row() const
{
    const bool hasCertainRow = std::abs(normalizedPosition(position(), false, true).y - position().y) < COORDINATE_TOLERANCE;
    const std::optional<unsigned int> result = hasCertainRow
        ? std::optional<unsigned int>(std::round(position().y / BOX_SIZE))
        : std::nullopt;
    return result;
}


void Object::normalizePosition(bool horizontal, bool vertical)
{
    const sf::Vector2f newPosition = normalizedPosition(position(), horizontal, vertical);
    m_sprite.setPosition(newPosition);
}


void Object::render(sf::RenderTarget &target, const sf::Transform &transform) const
{
    // FIXME: Перекрывать текстурой игрока текстуру мира.
    target.draw(m_sprite, transform);
}
