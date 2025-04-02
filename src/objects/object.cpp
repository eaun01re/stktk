#include "object.h"

#include <cmath>

#include "math.h"


namespace
{

/// Точность, используемая для определения номера строки и колонки объекта.
constexpr float COORDINATE_TOLERANCE = 0.01;

}


Object::Id Object::m_lastId = NULL_ID;


Object::Object()
    : m_id(m_lastId++)
{
    m_sprite.setPosition(0, 0);
}


Object::Object(const Object &object)
    : m_id(object.m_id)
{
}


Object::Id Object::id() const noexcept
{
    return m_id;
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
    const sf::Vector2f normalizedPosition = ::normalizedPosition(position(), true, false);
    if (std::abs(normalizedPosition.x - position().x) >= COORDINATE_TOLERANCE)
    {
        return std::nullopt;
    }
    const Coordinate coordinate = std::round(normalizedPosition.x / BOX_SIZE);
    if (coordinate >= BOXES_COLUMNS)
    {
        return std::nullopt;
    }
    return coordinate;
}


std::optional<Object::Coordinate> Object::row() const
{
    const sf::Vector2f normalizedPosition = ::normalizedPosition(position(), false, true);
    if (std::abs(normalizedPosition.y - position().y) >= COORDINATE_TOLERANCE)
    {
        return std::nullopt;
    }
    const Coordinate coordinate = std::round(normalizedPosition.y / BOX_SIZE);
    if (coordinate >= BOXES_ROWS)
    {
        return std::nullopt;
    }
    return coordinate;
}


void Object::normalizePosition(bool horizontal, bool vertical)
{
    const sf::Vector2f newPosition = normalizedPosition(position(), horizontal, vertical);
    m_sprite.setPosition(newPosition);
}


void Object::update(const Duration &elapsed)
{
    if (m_animation->update(elapsed))
    {
        // Если анимация обновила текстуру,
        // то спрайту назначается обновлённое значение.
        m_sprite.setTextureRect(mirrorVertical(m_animation->rect()));
    }
}


void Object::render(sf::RenderTarget &target, const sf::Transform &transform) const
{
    // FIXME: Перекрывать текстурой игрока текстуру мира.
    target.draw(m_sprite, transform);
}


sf::Vector2f Object::move(const Duration &elapsed)
{
    if (!isTolerant(m_speed))
    {
        return sf::Vector2f();
    }

    const double elapsedSeconds = elapsed.count();
    sf::Vector2f offset = multiply(m_speed, elapsedSeconds);
    if ((m_movementLength.x != 0 &&
         std::isless(m_movementLength.x, 0.0f) == std::isless(m_movementLength.x - offset.x, 0.0f)) ||
        (m_movementLength.y != 0 &&
         std::isless(m_movementLength.y, 0.0f) == std::isless(m_movementLength.y - offset.y, 0.0f)))
    {
        m_sprite.move(offset.x, offset.y);
        m_movementLength.x -= offset.x;
        m_movementLength.y -= offset.y;
    }
    else
    {
        offset = m_movementLength;
        m_sprite.move(offset.x, offset.y);
        moveFinished();
    }
    return offset;
}


void Object::moveFinished()
{
    normalizePosition(true, true);
    m_speed = sf::Vector2f();
    m_movementLength = sf::Vector2f();
}
