#include "animation.h"


Animation::Animation(
    const sf::Texture &texture,
    const sf::Vector2u &spritesQuantity,
    const AnimationMap &animations,
    const Duration &switchTime)
    : m_animations(animations)
    , m_switchTime(switchTime)
{
    m_currentRect.width = texture.getSize().x / float(spritesQuantity.y);
    m_currentRect.height = texture.getSize().y / float(spritesQuantity.x);
    m_currentSpritesIndices = m_animations.cbegin();
}


void Animation::setAnimationId(const AnimationId id)
{
    if (m_currentAnimationId == id)
    {
        return;
    }

    const auto it = m_animations.find(id);
    if (it == m_animations.cend())
    {
        return;
    }

    m_currentSpritesIndices = it;
    m_currentAnimationId = id;
    m_currentSpriteIndex = 0;
    // Принудительное обновление анимации после смены спрайтов.
    m_totalTime = m_switchTime;
}


unsigned int Animation::currentSpriteIndex() const noexcept
{
    return m_currentSpriteIndex;
}


const sf::IntRect& Animation::rect() const noexcept
{
    return m_currentRect;
}


void Animation::update(const Duration &elapsed)
{
    m_totalTime += elapsed;
    if (m_totalTime < m_switchTime)
    {
        return;
    }
    m_totalTime -= m_switchTime;

    ++m_currentSpriteIndex;
    const AnimationSpriteIndices &spritesIndices = m_currentSpritesIndices->second;
    if (m_currentSpriteIndex >= spritesIndices.size())
    {
        m_currentSpriteIndex = 0;
    }

    m_currentRect.top = spritesIndices[m_currentSpriteIndex].row * m_currentRect.height;
    if (spritesIndices[m_currentSpriteIndex].mirrored)
    {
        m_currentRect.left = (spritesIndices[m_currentSpriteIndex].column + 1) * std::abs(m_currentRect.width);
        m_currentRect.width = -std::abs(m_currentRect.width);
    }
    else
    {
        m_currentRect.width = std::abs(m_currentRect.width);
        m_currentRect.left = spritesIndices[m_currentSpriteIndex].column * m_currentRect.width;
    }
}
