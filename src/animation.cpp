#include "animation.h"


Animator::Animator(
    const sf::Texture &texture,
    const sf::Vector2u &spritesQuantity,
    const AnimationMap &animations)
    : m_animations(animations)
{
    m_currentRect.width = texture.getSize().x / float(spritesQuantity.y);
    m_currentRect.height = texture.getSize().y / float(spritesQuantity.x);
    m_currentSpritesIndices = m_animations.cbegin();
}


void Animator::setAnimation(const AnimationOriented animation)
{
    setAnimationSequence({ animation });
}


void Animator::setAnimationSequence(
    const std::vector<AnimationOriented> &sequence)
{
    if (m_currentAnimationSequence == sequence)
    {
        return;
    }

    m_currentAnimationSequence = sequence;
    if (!m_currentAnimationSequence.empty())
    {
        applyAnimationId(m_currentAnimationSequence[0]);
    }
}


unsigned int Animator::currentSpriteIndex() const noexcept
{
    return m_currentSpriteIndex;
}


const sf::IntRect& Animator::rect() const noexcept
{
    return m_currentRect;
}


void Animator::applyAnimationId(const AnimationOriented &animation)
{
    const auto it = m_animations.find(animation.id);
    if (it == m_animations.cend())
    {
        return;
    }

    m_currentSpritesIndices = it;
    m_currentAnimationIndex = 0;
    m_currentSpriteIndex = 0;
    // Принудительное обновление анимации после смены спрайтов.
    m_forceUpdate = true;
}


void Animator::applyNextAnimation()
{
    if (m_currentAnimationIndex >= m_currentAnimationSequence.size() - 1)
    {
        return;
    }

    ++m_currentAnimationIndex;
    applyAnimationId(m_currentAnimationSequence[m_currentAnimationIndex]);
}


bool Animator::update(const Duration &elapsed)
{
    const TextureSpriteIndices &spritesIndices = m_currentSpritesIndices->second;

    if (!m_forceUpdate)
    {
        if (spritesIndices.size() <= 1 && m_currentAnimationSequence.size() == 1)
        {
            return false;
        }

        const Duration &swtichTime =
            spritesIndices.at(m_currentSpriteIndex).duration;
        m_totalTime += elapsed;
        if (m_totalTime < swtichTime)
        {
            return false;
        }
        m_totalTime -= swtichTime;

        ++m_currentSpriteIndex;
    }
    m_forceUpdate = false;

    if (m_currentSpriteIndex >= spritesIndices.size())
    {
        // Переход к первому спрайту.
        m_currentSpriteIndex = 0;

        applyNextAnimation();
    }

    setSpriteIndex(spritesIndices[m_currentSpriteIndex]);
    return true;
}


void Animator::setSpriteIndex(const TextureSpriteIndex &index)
{
    m_currentRect.top = index.row * m_currentRect.height;
    const bool mirrored =
        index.mirrored != m_currentAnimationSequence[m_currentAnimationIndex].mirrored;
    if (mirrored)
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
