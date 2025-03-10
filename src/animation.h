#pragma once


#include <vector>

#include <SFML/Graphics.hpp>

#include "clock.h"


/// Положение фрагмента текстуры (спрайта) в таблице.
/// Размеры фрагмента считаются известными.
struct TextureSpriteIndex
{
    /*!
     * \param[in] row Номер строки в таблице спрайтов.
     * \param[in] column Номер колонки в таблице спрайтов.
     * \param[in] mirrored Признак отражения по горизонтали.
     */
    TextureSpriteIndex(unsigned int row, unsigned int column, bool mirrored)
        : row(row)
        , column(column)
        , mirrored(mirrored)
    {
    }

    unsigned int row;
    unsigned int column;
    bool mirrored{ false };
};

using AnimationSpriteIndices = std::vector<TextureSpriteIndex>;


class Animation final
{
public:
    explicit Animation(
        const sf::Texture &texture,
        const sf::Vector2u &spritesQuantity,
        const Duration &switchTime);
    void setSpritesIndices(const AnimationSpriteIndices &indices);
    void update(const Duration &elapsed);
    unsigned int currentSpriteIndex() const noexcept;
    const sf::IntRect& rect() const noexcept;

private:
    const Duration m_switchTime;

    AnimationSpriteIndices m_spritesIndices;
    sf::Vector2u m_currentSprite;
    unsigned int m_currentSpriteIndex{ 0 };

    sf::IntRect m_rect;

    Duration m_totalTime;
};
