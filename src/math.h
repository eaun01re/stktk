#pragma once


#include <SFML/Graphics.hpp>


constexpr float TOLERANCE = 1.0e-2;


bool isTolerant(const sf::Vector2f &vector);

sf::Vector2f multiply(const sf::Vector2f &vector, double a);

/// Применяется для вертикального отражение текстуры.
/// https://en.sfml-dev.org/forums/index.php?topic=12947.0
sf::IntRect mirrorVertical(const sf::IntRect &rect);

/*!
 * Возвращает округлённое до целого количества размеров ящика положение.
 * Нормализация необходима после окончания движения.
 * \param[in] horizontal Выполнять нормализацию по горизонтали.
 * \param[in] vertical Выполнять нормализацию по вертикали.
 */
sf::Vector2f normalizedPosition(
    const sf::Vector2f &position,
    bool horizontal,
    bool vertical);
