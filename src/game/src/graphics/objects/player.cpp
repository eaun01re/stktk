#include <game/graphics/objects/player.h>

#include <random>

#include <game/log.h>
#include <game/graphics/objects/object.h>
#include <game/resource_loader.h>

#include "math/math.h"


namespace
{

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_SIZE(5, 3);

const TextureSpriteIndices ANIMATION_IDLE
{
    { 0, 0, false },
    { 0, 1, false },
    { 0, 2, false },
    { 0, 0, false },
    { 0, 1, false },
    { 0, 2, true }
};
const TextureSpriteIndices ANIMATION_WALK{ { 1, 0 }, { 1, 1 }, { 1, 2 } };
const TextureSpriteIndices ANIMATION_PUSH{ { 2, 0 }, { 2, 1 }, { 2, 2 } };
const TextureSpriteIndices ANIMATION_JUMP{ { 3, 0 } };
const TextureSpriteIndices ANIMATION_DYING{ { 4, 0 } };
const TextureSpriteIndices ANIMATION_DEAD{ { 4, 1 }, { 4, 2 } };

}


unsigned int Player::height()
{
    return BOX_SIZE * 2;
}


Player::Player(const MoveFinishedCallback &callback)
    : m_moveFinishedCallback(callback)
{
    init();
}


void Player::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    setTexture(*resourceLoader.texture(ResourceLoader::TextureId::Player));

    m_animator.reset(new Animator(*getTexture(), TEXTURE_SIZE));
    setAnimation(AnimationOriented(&ANIMATION_IDLE));
    setTextureRect(mirrorVertical(m_animator->rect()));
    setColor(BACKGROUND_COLOR);
}


void Player::update(const Duration &elapsed)
{
    Object::update(elapsed);

    if (!isMoving())
    {
        m_lookLeft = m_animator->currentSpriteIndex() < ANIMATION_IDLE.size() / 2;
    }

    Object::move(elapsed);
}

void Player::move(Direction direction, bool push)
{
    setDirection(direction, push);
}


Player::Direction Player::direction() const noexcept
{
    return m_direction;
}


void Player::idle()
{
    if (alive())
    {
        setAnimation(AnimationOriented(&ANIMATION_IDLE));
    }
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
            AnimationOriented(&ANIMATION_DYING),
            AnimationOriented(&ANIMATION_DEAD, left)
        });
}


void Player::moveFinished()
{
    Object::moveFinished();
    m_moveFinishedCallback(m_direction);
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
        LOG_WARNING(
            "Attempt to set incorrect player direction: "
            << int(direction) << '.');
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
    setAnimation(AnimationOriented(
        &animationByDirection(directions, push),
        !m_lookLeft));
}


const TextureSpriteIndices &Player::animationByDirection(
    const sf::Vector2f &directions,
    bool push) const noexcept
{
    if (!m_alive)
    {
        return ANIMATION_DEAD;
    }
    if (directions.y != 0)
    {
        return ANIMATION_JUMP;
    }
    return push ? ANIMATION_PUSH : ANIMATION_WALK;
}
