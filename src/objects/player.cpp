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
const sf::Vector2u TEXTURE_SIZE(3, 4);

const std::map<AnimationId, TextureSpriteIndices> ANIMATIONS
{
    {
        Player::Animations::Idle,
        TextureSpriteIndices
        {
            { 0, 0, false },
            { 0, 1, false },
            { 0, 2, false },
            { 0, 0, false },
            { 0, 1, false },
            { 0, 2, true }
        }
    },
    {
        Player::Animations::WalkLeft,
        TextureSpriteIndices
        {
            { 1, 0, false },
            { 1, 1, false },
            { 1, 2, false }
        }
    },
    {
        Player::Animations::WalkRight,
        TextureSpriteIndices
        {
            { 1, 0, true },
            { 1, 1, true },
            { 1, 2, true }
        }
    },
    {
        Player::Animations::PushLeft,
        TextureSpriteIndices
        {
            { 2, 0, false },
            { 2, 1, false },
            { 2, 2, false }
        }
    },
    {
        Player::Animations::PushRight,
        TextureSpriteIndices
        {
            { 2, 0, true },
            { 2, 1, true },
            { 2, 2, true }
        }
    },
    {
        Player::Animations::JumpLeft,
        TextureSpriteIndices
        {
            { 0, 3, false }
        }
    },
    {
        Player::Animations::JumpRight,
        TextureSpriteIndices
        {
            { 0, 3, true }
        }
    },
};

}


void Player::init(const sf::Texture &texture)
{
    Object::init(texture);

    m_animation.reset(new Animation(
        m_texture,
        TEXTURE_SIZE,
        ANIMATIONS,
        ANIMATION_INTERVAL));
    m_animation->setAnimationId(Animations::Idle);
    m_sprite.setTextureRect(mirrorVertical(m_animation->rect()));
}


void Player::update(const Duration &elapsed)
{
    Object::update(elapsed);

    if (!isMoving())
    {
        m_lookLeft = m_animation->currentSpriteIndex() < ANIMATIONS.at(Animations::Idle).size() / 2;
    }

    Object::move(elapsed);
}


void Player::move(Direction direction, bool push, Direction nextDirection)
{
    setDirection(direction, push);
    m_nextDirection = nextDirection;
}


void Player::stop()
{
    m_animation->setAnimationId(Animations::Idle);
}


void Player::moveFinished()
{
    Object::moveFinished();
    move(m_nextDirection, Direction::None);
}


Player::Direction Player::direction() const noexcept
{
    return m_direction;
}


void Player::setDirection(Direction direction, bool push)
{
    m_speed = sf::Vector2f();
    m_movementLength = sf::Vector2f();
    m_direction = direction;

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

    // Обновление направления взгляда.
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
        m_animation->setAnimationId(push
            ? Animations::PushLeft
            : Animations::WalkLeft);
        break;
    case Direction::Right:
        m_speed = sf::Vector2f(SPEED, 0);
        m_movementLength = sf::Vector2f(BOX_SIZE, 0);
        m_animation->setAnimationId(push
            ? Animations::PushRight
            : Animations::WalkRight);
        break;
    case Direction::Up:
        m_speed = sf::Vector2f(0, SPEED);
        m_movementLength = sf::Vector2f(0, BOX_SIZE);
        m_animation->setAnimationId(m_lookLeft
            ? Animations::JumpLeft
            : Animations::JumpRight);
        break;
    case Direction::Down:
        m_speed = sf::Vector2f(0, -SPEED);
        m_movementLength = sf::Vector2f(0, -std::numeric_limits<float>::infinity());
        m_animation->setAnimationId(m_lookLeft
            ? Animations::JumpLeft
            : Animations::JumpRight);
        break;
    case Direction::UpLeft:
        m_speed = sf::Vector2f(-SPEED, SPEED);
        m_movementLength = sf::Vector2f(-BOX_SIZE, BOX_SIZE);
        m_animation->setAnimationId(Animations::JumpLeft);
        break;
    case Direction::UpRight:
        m_speed = sf::Vector2f(SPEED, SPEED);
        m_movementLength = sf::Vector2f(BOX_SIZE, BOX_SIZE);
        m_animation->setAnimationId(Animations::JumpRight);
        break;
    case Direction::DownLeft:
        m_speed = sf::Vector2f(-SPEED, -SPEED);
        m_movementLength = sf::Vector2f(-BOX_SIZE, -BOX_SIZE);
        m_animation->setAnimationId(Animations::JumpLeft);
        break;
    case Direction::DownRight:
        m_speed = sf::Vector2f(SPEED, -SPEED);
        m_movementLength = sf::Vector2f(BOX_SIZE, -BOX_SIZE);
        m_animation->setAnimationId(Animations::JumpRight);
        break;
    }
}
