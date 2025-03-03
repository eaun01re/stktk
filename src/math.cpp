#include "math.h"

#include <cmath>

#include "box.h"


bool isTolerant(const sf::Vector2f &vector)
{
    return (std::max(std::abs(vector.x), std::abs(vector.y)) >= TOLERANCE);
}


sf::IntRect mirrorVertical(const sf::IntRect &rect)
{
    const sf::IntRect result(
        rect.left,
        rect.top + rect.height,
        rect.width,
        -rect.height);
    return result;
}


sf::Vector2f normalizedPosition(
    const sf::Vector2f &position,
    bool horizontal,
    bool vertical)
{
    sf::Vector2f result = position;
    if (horizontal)
    {
        const float column = result.x / BOX_SIZE;
        result.x = std::round(column) * BOX_SIZE;
    }
    if (vertical)
    {
        const float row = result.y / BOX_SIZE;
        result.y = std::round(row) * BOX_SIZE;
    }
    return result;
}
