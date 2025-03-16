#include "box.h"

#include <cmath>
#include <random>
#include <limits>

#include "math.h"
#include "objects/object.h"


namespace
{

/// Длительность отображения одного кадра анимации.
const Duration ANIMATION_INTERVAL = std::chrono::milliseconds(300);

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_SIZE(1, 11);

const std::map<AnimationId, TextureSpriteIndices> ANIMATIONS
{
    { Box::Animations::Rest0, TextureSpriteIndices{ { 0, 0 } } },
    { Box::Animations::Rest1, TextureSpriteIndices{ { 0, 1 } } },
    { Box::Animations::Rest2, TextureSpriteIndices{ { 0, 2 } } },
    { Box::Animations::Rest3, TextureSpriteIndices{ { 0, 3 } } },
    { Box::Animations::Rest4, TextureSpriteIndices{ { 0, 4 } } },
    { Box::Animations::Rest5, TextureSpriteIndices{ { 0, 5 } } },
    { Box::Animations::Rest6, TextureSpriteIndices{ { 0, 6 } } },
    { Box::Animations::Rest7, TextureSpriteIndices{ { 0, 7 } } },
    {
        Box::Animations::Blow,
        TextureSpriteIndices
        {
            { 0, 8, false },
            { 0, 9, false },
            { 0, 10, false }
        }
    }
};

Box::Animations randomRestStyle()
{
    static std::mt19937 randomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> distributionBoxStyle(
        Box::Animations::Rest0,
        Box::Animations::Rest7);
    return Box::Animations(distributionBoxStyle(randomEngine));
}

Duration blowDuration()
{
    return (ANIMATIONS.at(Box::Animations::Blow).size()) * ANIMATION_INTERVAL;
}

}


Box::Box(Id id)
    : m_id(id)
{
}


Box::Box(const Box &box)
    : m_id(box.id())
{
    init(box.m_texture);
    setPosition(box.position());
    setStyle(box.style());
}


void Box::init(const sf::Texture &texture)
{
    Object::init(texture);

    m_animation.reset(new Animation(
        m_texture,
        TEXTURE_SIZE,
        ANIMATIONS,
        ANIMATION_INTERVAL));
    m_sprite.setTextureRect(mirrorVertical(m_animation->rect()));
    m_sprite.setColor(BACKGROUND_COLOR);
    setStyle(randomRestStyle());
}


void Box::update(const Duration &elapsed)
{
    if (m_animation->update(elapsed))
    {
        m_sprite.setTextureRect(mirrorVertical(m_animation->rect()));
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
        m_speed = sf::Vector2f();
        m_movementLength = sf::Vector2f();
    }
}


Box::Id Box::id() const noexcept
{
    return m_id;
}


Box::Animations Box::style() const noexcept
{
    return m_style;
}


void Box::setStyle(Animations style)
{
    m_style = style;
    m_animation->setAnimationId(m_style);
}


void Box::move(Direction direction)
{
    m_direction = direction;
    switch (direction)
    {
    case Direction::None:
        break;
    case Direction::Left:
        m_speed = sf::Vector2f(-SPEED, 0);
        m_movementLength = sf::Vector2f(-BOX_SIZE, 0);
        break;
    case Direction::Right:
        m_speed = sf::Vector2f(SPEED, 0);
        m_movementLength = sf::Vector2f(BOX_SIZE, 0);
        break;
    case Direction::Down:
        m_speed = sf::Vector2f(0, -SPEED);
        m_movementLength = sf::Vector2f(0, -std::numeric_limits<float>::infinity());
        break;
    }
}


Box::Direction Box::direction() const noexcept
{
    return m_direction;
}


void Box::blow()
{
    if (m_blowStart.has_value())
    {
        return;
    }

    m_blowStart = std::chrono::steady_clock::now();
    setStyle(Box::Animations::Blow);
}


bool Box::isBlowed() const
{
    if (!m_blowStart.has_value())
    {
        return false;
    }

    const TimePoint blowStart = m_blowStart.value();
    const TimePoint now = Clock::now();
    const Duration elapsed = std::chrono::duration_cast<Duration>(now - blowStart);
    return elapsed > blowDuration();
}
