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
    m_dirty = true;
}


unsigned int Animation::currentSpriteIndex() const noexcept
{
    return m_currentSpriteIndex;
}


const sf::IntRect& Animation::rect() const noexcept
{
    return m_currentRect;
}


bool Animation::update(const Duration &elapsed)
{
    const TextureSpriteIndices &spritesIndices = m_currentSpritesIndices->second;

    if (!m_dirty)
    {
        if (spritesIndices.size() == 1)
        {
            return false;
        }

        m_totalTime += elapsed;
        if (m_totalTime < m_switchTime)
        {
            return false;
        }
        m_totalTime -= m_switchTime;

        ++m_currentSpriteIndex;
    }
    m_dirty = false;

    if (m_currentSpriteIndex >= spritesIndices.size())
    {
        m_currentSpriteIndex = 0;
    }
    setSpriteIndex(spritesIndices[m_currentSpriteIndex]);
    return true;
}


void Animation::setSpriteIndex(const TextureSpriteIndex &index)
{
    m_currentRect.top = index.row * m_currentRect.height;
    if (index.mirrored)
    {
        m_currentRect.left = (index.column + 1) * std::abs(m_currentRect.width);
        m_currentRect.width = -std::abs(m_currentRect.width);
    }
    else
    {
        m_currentRect.width = std::abs(m_currentRect.width);
        m_currentRect.left = index.column * m_currentRect.width;
    }
}
