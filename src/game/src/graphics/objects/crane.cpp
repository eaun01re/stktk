#include <game/graphics/objects/crane.h>

#include <cmath>

#include <game/resource_loader.h>

#include "math/math.h"


namespace
{

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_SIZE(1, 2);

/// Скорость движения крана, пиксели в секунду.
constexpr float CRANE_SPEED = SPEED;

const TextureSpriteIndices ANIMATION_HOLDING{ { 0, 0 } };
const TextureSpriteIndices ANIMATION_OPEN{ { 0, 1 } };

}


Crane::Crane()
{
    init();
}


void Crane::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    setTexture(*resourceLoader.texture(ResourceLoader::TextureId::Crane));

    m_animator.reset(new Animator(*getTexture(), TEXTURE_SIZE));
    setTextureRect(mirrorVertical(m_animator->rect()));
    setColor(BACKGROUND_COLOR);
}


void Crane::update(const Duration &elapsed)
{
    Object::update(elapsed);
    Object::move(elapsed);
}


int Crane::width() const noexcept
{
    return std::abs(m_animator->rect().width);
}


void Crane::reset(const sf::Vector2f &position, bool left, float movementLength)
{
    setPosition(position);
    setDirection(left, movementLength);
    setAnimation(AnimationOriented(&ANIMATION_HOLDING));
    m_readyToReset = false;
}


void Crane::stop()
{
    m_speed = sf::Vector2f();
}


void Crane::setDirection(bool left, float movementLength)
{
    m_left = left;
    const float directionMultiplier = m_left ? -1 : 1;
    m_speed = sf::Vector2f(directionMultiplier * CRANE_SPEED, 0);
    m_movementLength = sf::Vector2f(directionMultiplier * movementLength, 0);
}


bool Crane::isLeft() const noexcept
{
    return m_left;
}


Object::Id Crane::boxId() const noexcept
{
    return m_boxId;
}


void Crane::load(Id boxId)
{
    m_boxId = boxId;
}


bool Crane::isLoaded() const noexcept
{
    return m_boxId != NULL_ID;
}


Coordinate Crane::dropColumn() const noexcept
{
    return m_dropColumn;
}


void Crane::setDropColumn(Coordinate column)
{
    m_dropColumn = column;
}


void Crane::drop()
{
    m_boxId = NULL_ID;
    m_animator->setAnimationSequence(
    {
        AnimationOriented(&ANIMATION_OPEN),
        AnimationOriented(&ANIMATION_HOLDING)
    });
}


bool Crane::readyToReset() const noexcept
{
    return m_readyToReset;
}


void Crane::moveFinished()
{
    m_readyToReset = true;
}
