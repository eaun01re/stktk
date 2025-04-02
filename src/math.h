#pragma once


#include <SFML/Graphics.hpp>


constexpr float TOLERANCE = 1.0e-2;


bool isTolerant(const sf::Vector2f &vector);

sf::Vector2f multiply(const sf::Vector2f &vector, double a);

template<typename T>
sf::Vector2<T> sum(const sf::Vector2<T> &a, const sf::Vector2<T> &b)
{
    return sf::Vector2<T>(a.x + b.x, a.y + b.y);
}

/// Применяется для вертикального отражение текстуры.
/// https://en.sfml-dev.org/forums/index.php?topic=12947.0
sf::IntRect mirrorVertical(const sf::IntRect &rect);

/*!
 * Возвращает округлённое до целого количества размеров ящика положение.
 * Нормализация необходима после окончания движения.
 * \param[in] position Округляемое положение.
 * \param[in] horizontal Выполнять нормализацию по горизонтали.
 * \param[in] vertical Выполнять нормализацию по вертикали.
 */
sf::Vector2f normalizedPosition(
    const sf::Vector2f &position,
    bool horizontal,
    bool vertical);
