#include <game/animation.h>
#include <game/log.h>


Animator::Animator(
    const sf::Texture &texture,
    const sf::Vector2u &spritesQuantity)
{
    m_currentRect.width = texture.getSize().x / float(spritesQuantity.y);
    m_currentRect.height = texture.getSize().y / float(spritesQuantity.x);
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
        setAnimationIndex(0);
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


void Animator::setAnimationIndex(const std::size_t index)
{
    m_currentAnimationIndex = index;
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

    setAnimationIndex(m_currentAnimationIndex + 1);
}


bool Animator::update(const Duration &elapsed)
{
    const TextureSpriteIndicesPtr indicesPtr =
        m_currentAnimationSequence[m_currentAnimationIndex].indices;
    if (indicesPtr == nullptr)
    {
        LOG_WARNING("Null texture sprite indices.");
        return false;
    }
    const TextureSpriteIndices &spritesIndices = *indicesPtr;

    if (!m_forceUpdate)
    {
        if (spritesIndices.size() <= 1 && m_currentAnimationSequence.size() == 1)
        {
            return false;
        }

        const Duration &swtichTime =
            spritesIndices.at(m_currentSpriteIndex).duration;
        m_currentSpriteTime += elapsed;
        if (m_currentSpriteTime < swtichTime)
        {
            return false;
        }
        m_currentSpriteTime -= swtichTime;

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
