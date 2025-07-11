#include <game/graphics/objects/object.h>

#include <cmath>

#include "math/math.h"


namespace
{

/// Точность, используемая для определения номера строки и колонки объекта.
constexpr float COORDINATE_TOLERANCE = 0.00001;

/*!
 * Возвращает нормализованную величину смещения, на которую должен
 * переместиться объект.
 * \param[in] movementLength Общее количество движения, назначенное объекту.
 * \param[in] offset Смещение, на которое должен переместиться объект
 * за один такт обновления.
 * \return Нормализованная величинf смещения, обеспечивающая точное
 * позиционирование.
 */
float normalizedOffset(const float movementLength, const float offset)
{
    if (movementLength == 0)
    {
        return 0;
    }

    float result = 0;
    const float movementLengthLeft = movementLength - offset;
    // Если после вычета смещения из количества движения, назначенного объекту,
    // знак последнего не изменился и его модуль больше погрешности,
    // то движение еще не завершено.
    if (std::isless(movementLength, 0.0f) ==
        std::isless(movementLengthLeft, 0.0f) &&
        std::abs(movementLengthLeft) > COORDINATE_TOLERANCE)
    {
        result = offset;
    }
    else
    {
        result = movementLength;
    }
    return result;
}

}


Object::Id Object::m_lastId = NULL_ID;


Object::Object()
    : m_id(m_lastId++)
{
}


Object::Object(const Object &object)
    : sf::Sprite()
    , m_id(object.m_id)
{
}


void Object::update(const Duration &elapsed)
{
    if (m_animator->update(elapsed))
    {
        // Если анимация обновила текстуру,
        // то спрайту назначается обновлённое значение.
        setTextureRect(mirrorVertical(m_animator->rect()));
    }
}


Object::Id Object::id() const noexcept
{
    return m_id;
}


const sf::Vector2f& Object::speed() const noexcept
{
    return m_speed;
}


bool Object::isFalling() const noexcept
{
    return m_speed.y < 0;
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

    if (!isTolerant(m_speed))
    {
        moveFinished();
    }
}


Coordinates Object::coordinates() const
{
    Coordinates result;
    result.row = this->row();
    result.column = this->column();
    return result;
}


std::optional<Coordinate> Object::column() const
{
    const sf::Vector2f normalizedPosition =
        ::normalizedPosition(getPosition(), true, false);
    if (std::abs(normalizedPosition.x - getPosition().x) >= COORDINATE_TOLERANCE)
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


std::optional<Coordinate> Object::row() const
{
    const sf::Vector2f normalizedPosition =
        ::normalizedPosition(getPosition(), false, true);
    if (std::abs(normalizedPosition.y - getPosition().y) >= COORDINATE_TOLERANCE)
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
    const sf::Vector2f newPosition =
        normalizedPosition(getPosition(), horizontal, vertical);
    setPosition(newPosition);
}


void Object::setAnimation(const AnimationOriented &animation)
{
    setAnimationSequence({ animation });
}


void Object::setAnimationSequence(const std::vector<AnimationOriented> &ids)
{
    m_currentAnimationId = 0;
    m_animationIds = ids;
    m_animator->setAnimation(m_animationIds[m_currentAnimationId]);
}


void Object::move(const Duration &elapsed)
{
    if (!isTolerant(m_speed))
    {
        return;
    }

    if (!m_moving)
    {
        moveStarted();
    }

    const double elapsedSeconds = elapsed.count();
    const sf::Vector2f offset = multiply(m_speed, elapsedSeconds);
    const sf::Vector2f normalizedOffset(
        ::normalizedOffset(m_movementLength.x, offset.x),
        ::normalizedOffset(m_movementLength.y, offset.y));
    sf::Sprite::move(normalizedOffset);
    m_movementLength -= normalizedOffset;

    if (isNull(m_movementLength))
    {
        moveFinished();
    }
}


void Object::moveStarted()
{
    m_moving = true;
}


void Object::moveFinished()
{
    m_moving = false;
    normalizePosition(true, true);
    m_speed = sf::Vector2f();
    m_movementLength = sf::Vector2f();
}
