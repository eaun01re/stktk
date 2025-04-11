#include "player.h"

#include <cmath>
#include <random>

#include "log.h"
#include "math.h"
#include "objects/object.h"


namespace
{

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_SIZE(5, 3);

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
        Player::Animations::Walk,
        TextureSpriteIndices{ { 1, 0 }, { 1, 1 }, { 1, 2 } }
    },
    {
        Player::Animations::Push,
        TextureSpriteIndices{ { 2, 0 }, { 2, 1 }, { 2, 2 } }
    },
    {
        Player::Animations::Jump,
        TextureSpriteIndices{ { 3, 0 } }
    },
    {
        Player::Animations::Dying,
        TextureSpriteIndices{ { 4, 0 } }
    },
    {
        Player::Animations::Dead,
        TextureSpriteIndices{ { 4, 1 }, { 4, 2 } }
    },
};

}


unsigned int Player::height()
{
    return BOX_SIZE * 2;
}


void Player::init(const sf::Texture &texture)
{
    Object::init(texture);

    m_animator.reset(new Animator(
        m_texture,
        TEXTURE_SIZE,
        ANIMATIONS));
    setAnimationId(AnimationOriented(Animations::Idle));
    m_sprite.setTextureRect(mirrorVertical(m_animator->rect()));
    m_sprite.setColor(BACKGROUND_COLOR);
}


void Player::update(const Duration &elapsed)
{
    Object::update(elapsed);

    if (!isMoving())
    {
        m_lookLeft = m_animator->currentSpriteIndex() < ANIMATIONS.at(Animations::Idle).size() / 2;
    }

    Object::move(elapsed);
}


void Player::move(Direction direction, bool push, Direction nextDirection)
{
    setDirection(direction, push);
    m_nextDirection = nextDirection;
}


Player::Direction Player::direction() const noexcept
{
    return m_direction;
}


void Player::stop()
{
    setAnimationId(AnimationOriented(Animations::Idle));
}


void Player::die()
{
    std::random_device device;
    std::mt19937 randomEngine = std::mt19937(device());
    std::uniform_int_distribution<std::mt19937::result_type> distributionDirection(0, 1);
    const bool left = distributionDirection(randomEngine) == 0;

    m_animator->setAnimationSequence(
        {
            AnimationOriented(Animations::Dying),
            AnimationOriented(Animations::Dead, left)
        });
}


void Player::moveFinished()
{
    Object::moveFinished();
    move(m_nextDirection, Direction::None);
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
        setAnimationId(push
            ? AnimationOriented(Animations::Push, false)
            : AnimationOriented(Animations::Walk, false));
        break;
    case Direction::Right:
        m_speed = sf::Vector2f(SPEED, 0);
        m_movementLength = sf::Vector2f(BOX_SIZE, 0);
        setAnimationId(push
            ? AnimationOriented(Animations::Push, true)
            : AnimationOriented(Animations::Walk, true));
        break;
    case Direction::Up:
        m_speed = sf::Vector2f(0, SPEED);
        m_movementLength = sf::Vector2f(0, BOX_SIZE);
        setAnimationId(AnimationOriented(Animations::Jump, m_lookLeft));
        break;
    case Direction::Down:
        m_speed = sf::Vector2f(0, -SPEED);
        m_movementLength = sf::Vector2f(0, -std::numeric_limits<float>::infinity());
        setAnimationId(AnimationOriented(Animations::Jump, m_lookLeft));
        break;
    case Direction::UpLeft:
        m_speed = sf::Vector2f(-SPEED, SPEED);
        m_movementLength = sf::Vector2f(-BOX_SIZE, BOX_SIZE);
        setAnimationId(AnimationOriented(Animations::Jump, false));
        break;
    case Direction::UpRight:
        m_speed = sf::Vector2f(SPEED, SPEED);
        m_movementLength = sf::Vector2f(BOX_SIZE, BOX_SIZE);
        setAnimationId(AnimationOriented(Animations::Jump, true));
        break;
    case Direction::DownLeft:
        m_speed = sf::Vector2f(-SPEED, -SPEED);
        m_movementLength = sf::Vector2f(-BOX_SIZE, -BOX_SIZE);
        setAnimationId(AnimationOriented(Animations::Jump, false));
        break;
    case Direction::DownRight:
        m_speed = sf::Vector2f(SPEED, -SPEED);
        m_movementLength = sf::Vector2f(BOX_SIZE, -BOX_SIZE);
        setAnimationId(AnimationOriented(Animations::Jump, true));
        break;
    }
}
