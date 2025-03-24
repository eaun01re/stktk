#include "crane.h"

#include <cmath>

#include "math.h"
#include "objects/object.h"


namespace
{

/// Длительность отображения одного кадра анимации.
const Duration ANIMATION_INTERVAL = std::chrono::milliseconds(300);

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_SIZE(1, 2);

/// Скорость движения крана, пиксели в секунду.
constexpr float CRANE_SPEED = SPEED;

const std::map<AnimationId, TextureSpriteIndices> ANIMATIONS
{
    { Crane::State::Holding, TextureSpriteIndices { { 0, 0 } } },
    { Crane::State::Open, TextureSpriteIndices { { 0, 1 } } }
};

}


Crane::Crane(const Crane &crane)
    : Object(crane)
{
    init(crane.m_texture);
    setPosition(crane.position());
    setDirection(crane.isLeft());
}


void Crane::init(const sf::Texture &texture)
{
    Object::init(texture);

    m_animation.reset(new Animation(
        m_texture,
        TEXTURE_SIZE,
        ANIMATIONS,
        ANIMATION_INTERVAL));
    m_animation->setAnimationId(State::Holding);
    m_sprite.setTextureRect(mirrorVertical(m_animation->rect()));
    m_sprite.setColor(BACKGROUND_COLOR);
}


void Crane::update(const Duration &elapsed)
{
    Object::update(elapsed);
    Object::move(elapsed);
}


float Crane::width() const noexcept
{
    return m_animation->rect().width;
}


void Crane::setDirection(bool left)
{
    m_left = left;
    const float directionMultiplier = m_left ? -1 : 1;
    m_speed = sf::Vector2f(directionMultiplier * CRANE_SPEED, 0);
    m_movementLength = sf::Vector2f(directionMultiplier * BOX_SIZE, 0);
}


bool Crane::isLeft() const noexcept
{
    return m_left;
}
