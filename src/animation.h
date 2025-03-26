#pragma once


#include <vector>

#include <SFML/Graphics.hpp>

#include "clock.h"


namespace
{

/// Длительность отображения одного кадра анимации.
const Duration ANIMATION_INTERVAL_DEFAULT = std::chrono::milliseconds(300);
/// Длительность отображения кадра, обозначающая остановку анимации.
const Duration ANIMATION_INTERVAL_INFINITY(0);

}


/// Положение фрагмента текстуры (спрайта) в таблице.
/// Размеры фрагмента считаются известными.
struct TextureSpriteIndex
{
    /*!
     * \param[in] row Номер строки в таблице спрайтов.
     * \param[in] column Номер колонки в таблице спрайтов.
     * \param[in] mirrored Признак отражения по горизонтали.
     */
    TextureSpriteIndex(
        unsigned int row,
        unsigned int column,
        bool mirrored = false,
        Duration duration = ANIMATION_INTERVAL_DEFAULT)
        : row(row)
        , column(column)
        , mirrored(mirrored)
        , duration(duration)
    {
    }

    unsigned int row;
    unsigned int column;
    bool mirrored;
    Duration duration;
};

using AnimationId = unsigned int;
/// Номера спрайтов, составляющих одну определенную анимацию.
using TextureSpriteIndices = std::vector<TextureSpriteIndex>;
/// Список всех возможных анимаций объекта.
using AnimationMap = std::map<AnimationId, TextureSpriteIndices>;


class Animation final
{
public:
    explicit Animation(
        const sf::Texture &texture,
        const sf::Vector2u &spritesQuantity,
        const AnimationMap &animations);
    void setAnimationId(const AnimationId id);
    bool update(const Duration &elapsed);
    unsigned int currentSpriteIndex() const noexcept;
    const sf::IntRect& rect() const noexcept;

private:
    void setSpriteIndex(const TextureSpriteIndex &index);

private:
    const AnimationMap &m_animations;

    AnimationId m_currentAnimationId{ 0 };
    AnimationMap::const_iterator m_currentSpritesIndices;
    unsigned int m_currentSpriteIndex{ 0 };
    /// Участок текущего спрайта в текстуре.
    sf::IntRect m_currentRect;
    /// Признак принудительного обновления спрайта.
    bool m_dirty{ false };

    Duration m_totalTime;
};
