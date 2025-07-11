#pragma once


#include <vector>

#include <SFML/Graphics.hpp>

#include "clock.h"


namespace
{

/// Длительность отображения одного кадра анимации.
const Duration ANIMATION_INTERVAL_DEFAULT = std::chrono::milliseconds(300);

}


/// Положение фрагмента текстуры (спрайта) в таблице.
/// Размеры фрагмента считаются известными.
struct TextureSpriteIndex
{
    /*!
     * \param[in] row Номер строки в таблице спрайтов.
     * \param[in] column Номер колонки в таблице спрайтов.
     * \param[in] mirrored Признак отражения по горизонтали.
     * \param[in] duration Длительность отображения спрайта.
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

    bool operator==(const TextureSpriteIndex &other) const noexcept
    {
        return
            row == other.row && column == other.column &&
            mirrored == other.mirrored && duration == other.duration;
    }

    unsigned int row;
    unsigned int column;
    bool mirrored;
    Duration duration;
};

/// Номера спрайтов, составляющих одну определенную анимацию.
using TextureSpriteIndices = std::vector<TextureSpriteIndex>;
using TextureSpriteIndicesPtr = const TextureSpriteIndices*;

/// Анимация, составленная набором спрайтов и их ориентацией по горизонтали.
struct AnimationOriented
{
    /*!
     * \param[in] indices Список спрайтов.
     * \param[in] mirrored Признак отражения по горизонтали.
     */
    explicit AnimationOriented(
        TextureSpriteIndicesPtr indices,
        bool mirrored = false)
        : indices(indices)
        , mirrored(mirrored)
    {
    }

    bool operator==(const AnimationOriented &other) const noexcept
    {
        return indices == other.indices && mirrored == other.mirrored;
    }

    TextureSpriteIndicesPtr indices;
    /// Признак отражения спрайтов по горизонтали.
    bool mirrored;
};


class Animator final
{
public:
    /*!
     * \param[in] texture Текстура, содержащая спрайты.
     * \param[in] spritesQuantity Размерность текстуры в количестве спрайтов
     * по горизонтали и вертикали.
     */
    explicit Animator(
        const sf::Texture &texture,
        const sf::Vector2u &spritesQuantity);
    void setAnimation(const AnimationOriented animation);
    /*!
     * Назначает последовательность из нескольких анимаций.
     * После завершения одной анимации включается следующая,
     * если текущая анимация не последняя.
     * \param[in] sequence Последовательность анимаций.
     */
    void setAnimationSequence(const std::vector<AnimationOriented> &sequence);
    bool update(const Duration &elapsed);
    unsigned int currentSpriteIndex() const noexcept;
    const sf::IntRect& rect() const noexcept;

private:
    void setAnimationIndex(const std::size_t index);
    void applyNextAnimation();
    void setSpriteIndex(const TextureSpriteIndex &index);

private:
    std::size_t m_currentAnimationIndex{ 0 };
    std::vector<AnimationOriented> m_currentAnimationSequence;
    /// Номер текущего отображаемого спрайта.
    unsigned int m_currentSpriteIndex{ 0 };
    /// Участок текущего спрайта в текстуре.
    sf::IntRect m_currentRect;
    /// Признак принудительного обновления спрайта.
    /// Выставляется при назначении другой анимации.
    bool m_forceUpdate{ false };
    /// Длительность отображения текущего спрайта.
    Duration m_currentSpriteTime{ 0 };
};
