#include "animation.h"


Animation::Animation(
    const sf::Texture &texture,
    const sf::Vector2u &spritesQuantity,
    const Duration &switchTime)
    : m_spritesQuantity(spritesQuantity)
    , m_switchTime(switchTime)
{
    m_rect.width = texture.getSize().x / float(spritesQuantity.x);
    m_rect.height = texture.getSize().y / float(spritesQuantity.y);
}


void Animation::setSpritesIndices(const AnimationSpriteIndices &indices)
{
    m_spritesIndices = indices;
}


const sf::IntRect &Animation::rect() const
{
    return m_rect;
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
    if (m_currentSpriteIndex >= m_spritesIndices.size())
    {
        m_currentSpriteIndex = 0;
    }
    m_rect.left = m_spritesIndices[m_currentSpriteIndex].second * m_rect.width;
    m_rect.top = m_spritesIndices[m_currentSpriteIndex].first * m_rect.height;
}
