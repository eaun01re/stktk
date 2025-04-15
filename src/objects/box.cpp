#include "box.h"

#include <cmath>
#include <random>
#include <limits>

#include "math.h"
#include "resource_loader.h"


namespace
{

/// Длительность отображения одного кадра анимации.
const Duration ANIMATION_INTERVAL = ANIMATION_INTERVAL_DEFAULT;

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_SIZE(1, 11);

const std::vector<TextureSpriteIndices> ANIMATIONS_REST
{
    { { 0, 0 } },
    { { 0, 1 } },
    { { 0, 2 } },
    { { 0, 3 } },
    { { 0, 4 } },
    { { 0, 5 } },
    { { 0, 6 } },
    { { 0, 7 } }
};
const TextureSpriteIndices ANIMATION_BLOW
{
    { 0, 8, false, ANIMATION_INTERVAL },
    { 0, 9, false, ANIMATION_INTERVAL },
    { 0, 10, false, ANIMATION_INTERVAL }
};

TextureSpriteIndicesPtr randomRestStyle()
{
    static std::mt19937 randomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> distributionBoxStyle(
        0,
        ANIMATIONS_REST.size() - 1);
    return &ANIMATIONS_REST.at(distributionBoxStyle(randomEngine));
}

Duration blowDuration()
{
    return ANIMATION_BLOW.size() * ANIMATION_INTERVAL;
}

}


Box::Box()
{
    init();
}


void Box::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    setTexture(*resourceLoader.texture(ResourceLoader::TextureId::Box));

    m_animator.reset(new Animator(*getTexture(), TEXTURE_SIZE));
    setAnimation(AnimationOriented(randomRestStyle()));
    setTextureRect(mirrorVertical(m_animator->rect()));
    setColor(BACKGROUND_COLOR);
}


void Box::update(const Duration &elapsed)
{
    Object::update(elapsed);

    if (m_blowDuration.has_value())
    {
        m_blowDuration = m_blowDuration.value() + elapsed;
    }

    Object::move(elapsed);
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


bool Box::blow()
{
    if (m_blowDuration.has_value())
    {
        return false;
    }

    m_blowDuration = Duration();
    m_speed = sf::Vector2f();
    setAnimation(AnimationOriented(&ANIMATION_BLOW));
    return true;
}


bool Box::isBlowing() const noexcept
{
    return m_blowDuration.has_value() && m_blowDuration.value() <= blowDuration();
}


bool Box::isBlowed() const noexcept
{
    return m_blowDuration.has_value() && m_blowDuration.value() > blowDuration();
}
