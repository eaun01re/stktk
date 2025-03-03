#include "player.h"

#include <cmath>

#include "log.h"
#include "math.h"


namespace
{

/// Длительность отображения одного кадра анимации.
const Duration ANIMATION_INTERVAL = std::chrono::milliseconds(300);
const sf::Vector2u ANIMATION_SPRITES(4, 1);
const sf::Vector2u ANIMATION_IDLE(0, 4);
const AnimationSpriteIndices ANIMATION_IDLE2{ {0, 0}, {0, 1}, {0, 2}, {0, 0}, {0, 1}, {0, 3} };
const sf::Vector2u ANIMATION_WALKING_LEFT(1, 4);
const sf::Vector2u ANIMATION_WALKING_RIGHT(2, 4);
const sf::Vector2u ANIMATION_PUSHING_LEFT(3, 4);
const sf::Vector2u ANIMATION_PUSHING_RIGHT(4, 4);

}


Player::Player()
{
    m_sprite.setPosition(0, 0);
}


void Player::init(sf::Texture texture)
{
    m_texture = std::move(texture);
    m_sprite.setTexture(m_texture);
    m_animation.reset(new Animation(
        m_texture,
        ANIMATION_SPRITES,
        ANIMATION_INTERVAL));
    m_animation->setSpritesIndices(ANIMATION_IDLE2);
}


void Player::setPosition(const sf::Vector2f &position)
{
    m_sprite.setPosition(position);
}


sf::Vector2f Player::position() const
{
    return m_sprite.getPosition();
}


const sf::Vector2f& Player::speed() const
{
    return m_speed;
}


void Player::move(Direction direction)
{
    // if (isTolerant(m_speed))
    // {
    //     return;
    // }

    switch (direction)
    {
    case Direction::Left:
        m_speed = sf::Vector2f(-SPEED_HORIZONTAL, 0);
        m_movementLength = sf::Vector2f(-BOX_SIZE, 0);
        break;
    case Direction::UpLeft:
        m_speed = sf::Vector2f(-SPEED_HORIZONTAL, SPEED_VECTICAL);
        m_movementLength = sf::Vector2f(-2 * BOX_SIZE, BOX_SIZE);
        break;
    case Direction::Right:
        m_speed = sf::Vector2f(SPEED_HORIZONTAL, 0);
        m_movementLength = sf::Vector2f(BOX_SIZE, 0);
        break;
    case Direction::UpRight:
        m_speed = sf::Vector2f(SPEED_HORIZONTAL, SPEED_VECTICAL);
        m_movementLength = sf::Vector2f(2 * BOX_SIZE, BOX_SIZE);
        break;
    case Direction::Up:
        m_speed = sf::Vector2f(0, SPEED_VECTICAL);
        m_movementLength = sf::Vector2f(0, BOX_SIZE);
        break;
    }
}


void Player::fall()
{
    m_speed.y = -SPEED_VECTICAL;
}


bool Player::isFalling() const
{
    return m_speed.y < 0;
}


bool Player::isRising() const
{
    return m_movementLength.y > 0;
}


bool Player::isMoving() const
{
    return isTolerant(m_speed);
}


void Player::stopFalling()
{
    m_speed.y = 0;
    m_movementLength.y = 0;
    normalizePosition(false, true);
}


void Player::normalizePosition(bool horizontal, bool vertical)
{
    const sf::Vector2f newPosition = normalizedPosition(position(), horizontal, vertical);
    m_sprite.setPosition(newPosition);
}


void Player::update(const Duration &elapsed)
{
    m_animation->update(elapsed);

    m_sprite.setTextureRect(mirrorVertical(m_animation->rect()));

    if (!isTolerant(m_speed))
    {
        // тут не всегда надо выходить?
        return;
    }

    const double elapsedSeconds = elapsed.count();
    const sf::Vector2f offset(m_speed.x * elapsedSeconds, m_speed.y * elapsedSeconds);
    m_sprite.move(offset.x, offset.y);

    // Сокращение "длины" горзинтальной и вертикальной составляющих целевого
    // вектора смещения (m_movementLength), пока это сокращение не
    // приведет к смене знака у составляющей.
    if (std::isless(m_movementLength.x, 0.0f) == std::isless(m_movementLength.x - offset.x, 0.0f))
    {
        m_movementLength.x -= offset.x;
    }
    else
    {
        m_speed.x = 0;
        normalizePosition(true, false);
        m_movementLength.x = 0;
    }

    if (std::isless(m_movementLength.y, 0.0f) == std::isless(m_movementLength.y - offset.y, 0.0f))
    {
        m_movementLength.y -= offset.y;// копится значение пока игрок стоит!
    }
    else
    {
        // m_speed.y = 0;
        m_movementLength.y = 0;
        normalizePosition(false, true);
        fall();
    }
}


void Player::render(sf::RenderTarget &target, const sf::Transform &transform) const
{
    target.draw(m_sprite, transform);
}
