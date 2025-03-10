#include "player.h"

#include <cmath>

#include "log.h"
#include "math.h"
#include "objects/object.h"


namespace
{

/// Длительность отображения одного кадра анимации.
const Duration ANIMATION_INTERVAL = std::chrono::milliseconds(300);

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u ANIMATION_SPRITES(3, 4);

const AnimationSpriteIndices ANIMATION_IDLE
{
    { 0, 0, false },
    { 0, 1, false },
    { 0, 2, false },
    { 0, 0, false },
    { 0, 1, false },
    { 0, 2, true }
};
const AnimationSpriteIndices ANIMATION_WALK_LEFT
{
    { 1, 0, false },
    { 1, 1, false },
    { 1, 2, false }
};
const AnimationSpriteIndices ANIMATION_WALK_RIGHT
{
    { 1, 0, true },
    { 1, 1, true },
    { 1, 2, true }
};
const AnimationSpriteIndices ANIMATION_PUSH_LEFT
{
    { 2, 0, false },
    { 2, 1, false },
    { 2, 2, false }
};
const AnimationSpriteIndices ANIMATION_PUSH_RIGHT
{
    { 2, 0, true },
    { 2, 1, true },
    { 2, 2, true }
};
const AnimationSpriteIndices ANIMATION_JUMP_LEFT{ { 0, 3, false } };
const AnimationSpriteIndices ANIMATION_JUMP_RIGHT{ { 0, 3, true } };

}


void Player::init(const sf::Texture &texture)
{
    Object::init(texture);

    m_animation.reset(new Animation(
        m_texture,
        ANIMATION_SPRITES,
        ANIMATION_INTERVAL));
    m_animation->setSpritesIndices(ANIMATION_IDLE);
}


void Player::update(const Duration &elapsed)
{
    m_animation->update(elapsed);
    m_sprite.setTextureRect(mirrorVertical(m_animation->rect()));
    if (!isMoving())
    {
        m_lookLeft = m_animation->currentSpriteIndex() < ANIMATION_IDLE.size() / 2;
    }

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
        setDirection(m_nextDirection, false);
        m_nextDirection = Direction::None;
    }
}


void Player::move(Direction direction, bool push, Direction nextDirection)
{
    setDirection(direction, push);
    m_nextDirection = nextDirection;
}


void Player::setDirection(Direction direction, bool push)
{
    m_speed = sf::Vector2f();
    m_movementLength = sf::Vector2f();
    m_animation->setSpritesIndices(ANIMATION_IDLE);

    if ((direction & Direction::Left && direction & Direction::Right) ||
        (direction & Direction::Up && direction & Direction::Down))
    {
        // Некорректная комбинация направлений.
        LOG_WARNING("Attempt to set incorrect player direction: " << int(direction) << '.');
        return;
    }
    if (push && direction != Direction::Left && direction != Direction::Right)
    {
        // Запрещено толкать в выбранном направлении.
        LOG_WARNING("Unable to push to direction " << int(direction));
        return;
    }

    if (direction & Direction::Left || direction & Direction::Right)
    {
        m_lookLeft = direction & Direction::Left;
    }

    switch (direction)
    {
    case Direction::None:
        break;
    case Direction::Left:
        m_speed = sf::Vector2f(-SPEED, 0);
        m_movementLength = sf::Vector2f(-BOX_SIZE, 0);
        m_animation->setSpritesIndices(push
           ? ANIMATION_PUSH_LEFT
           : ANIMATION_WALK_LEFT);
        break;
    case Direction::Right:
        m_speed = sf::Vector2f(SPEED, 0);
        m_movementLength = sf::Vector2f(BOX_SIZE, 0);
        m_animation->setSpritesIndices(push
            ? ANIMATION_PUSH_RIGHT
            : ANIMATION_WALK_RIGHT);
        break;
    case Direction::Up:
        m_speed = sf::Vector2f(0, SPEED);
        m_movementLength = sf::Vector2f(0, BOX_SIZE);
        m_animation->setSpritesIndices(m_lookLeft
            ? ANIMATION_JUMP_LEFT
            : ANIMATION_JUMP_RIGHT);
        break;
    case Direction::Down:
        m_speed = sf::Vector2f(0, -SPEED);
        m_movementLength = sf::Vector2f(0, -BOX_SIZE);
        m_animation->setSpritesIndices(m_lookLeft
            ? ANIMATION_JUMP_LEFT
            : ANIMATION_JUMP_RIGHT);
        break;
    case Direction::UpLeft:
        m_speed = sf::Vector2f(-SPEED, SPEED);
        m_movementLength = sf::Vector2f(-BOX_SIZE, BOX_SIZE);
        m_animation->setSpritesIndices(ANIMATION_JUMP_LEFT);
        break;
    case Direction::UpRight:
        m_speed = sf::Vector2f(SPEED, SPEED);
        m_movementLength = sf::Vector2f(BOX_SIZE, BOX_SIZE);
        m_animation->setSpritesIndices(ANIMATION_JUMP_RIGHT);
        break;
    case Direction::DownLeft:
        m_speed = sf::Vector2f(-SPEED, -SPEED);
        m_movementLength = sf::Vector2f(-BOX_SIZE, -BOX_SIZE);
        m_animation->setSpritesIndices(ANIMATION_JUMP_LEFT);
        break;
    case Direction::DownRight:
        m_speed = sf::Vector2f(SPEED, -SPEED);
        m_movementLength = sf::Vector2f(BOX_SIZE, -BOX_SIZE);
        m_animation->setSpritesIndices(ANIMATION_JUMP_RIGHT);
        break;
    }
}
