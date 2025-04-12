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


bool Player::alive() const noexcept
{
    return m_alive;
}


void Player::setAlive(bool alive)
{
    m_alive = alive;

    if (m_alive)
    {
        return;
    }

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

    const bool left = direction & Direction::Left;
    const bool right = direction & Direction::Right;
    const bool up = direction & Direction::Up;
    const bool down = direction & Direction::Down;
    const sf::Vector2f directions(
        float(right) - float(left),
        float(up) - float(down));

    // Проверка корректности направления.
    if ((left && right) || (up && down))
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
    if (left || right)
    {
        m_lookLeft = left;
    }

    m_speed = multiply(directions, SPEED);
    m_movementLength.x = BOX_SIZE * directions.x;
    m_movementLength.y = direction != Direction::Down
        ? BOX_SIZE * directions.y
        : -std::numeric_limits<float>::infinity();
    setAnimationId(AnimationOriented(
        animationId(directions, push),
        !m_lookLeft));
}


Player::Animations Player::animationId(
    const sf::Vector2f &directions,
    bool push) const noexcept
{
    if (!m_alive)
    {
        return Player::Animations::Dead;
    }
    if (directions.y != 0)
    {
        return Player::Animations::Jump;
    }
    return push ? Animations::Push : Animations::Walk;
}
