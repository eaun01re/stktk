#include "animation.h"


Animation::Animation(
    const sf::Texture &texture,
    const sf::Vector2u &spritesQuantity,
    const Duration &switchTime)
    : m_switchTime(switchTime)
{
    m_rect.width = texture.getSize().x / float(spritesQuantity.y);
    m_rect.height = texture.getSize().y / float(spritesQuantity.x);
}


void Animation::setSpritesIndices(const AnimationSpriteIndices &indices)
{
    m_spritesIndices = indices;

    // Принудительное обновление анимации после смены спрайтов.
    // FIXME: Устранить подёргивание при назначении анимации.
    m_totalTime = m_switchTime;
}


unsigned int Animation::currentSpriteIndex() const noexcept
{
    return m_currentSpriteIndex;
}


const sf::IntRect& Animation::rect() const noexcept
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

    m_rect.top = m_spritesIndices[m_currentSpriteIndex].row * m_rect.height;
    if (m_spritesIndices[m_currentSpriteIndex].mirrored)
    {
        m_rect.left = (m_spritesIndices[m_currentSpriteIndex].column + 1) * std::abs(m_rect.width);
        m_rect.width = -std::abs(m_rect.width);
    }
    else
    {
        m_rect.width = std::abs(m_rect.width);
        m_rect.left = m_spritesIndices[m_currentSpriteIndex].column * m_rect.width;
    }
}
