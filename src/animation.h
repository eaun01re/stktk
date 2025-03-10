#pragma once


#include <vector>

#include <SFML/Graphics.hpp>

#include "clock.h"


/// Положение фрагмента текстуры (номер строки, номер столбца)
/// Размеры фрагмента считаются известными.
using TextureSpriteIndex = std::pair<unsigned int, unsigned int>;

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
    Duration m_switchTime;

    AnimationSpriteIndices m_spritesIndices;
    sf::Vector2u m_currentSprite;
    unsigned int m_currentSpriteIndex{ 0 };

    sf::IntRect m_rect;

    Duration m_totalTime;
};
