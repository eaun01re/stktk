#pragma once


#include <SFML/Graphics.hpp>

#include "clock.h"


namespace
{

constexpr unsigned char BOX_SIZE = 8;

}


class Box final
{
public:
    enum class Direction
    {
        Left,
        Right,
        Down
    };

public:
    Box();

    void init(const sf::Texture &texture, unsigned int style, const sf::Vector2f &position);
    sf::Vector2f position() const;
    void move(Direction direction);
    void fall();
    void update(const Duration &elapsed);
    /*!
     * Нормализует положение, делая координаты кратными размеру ящика.
     * Нормализация необходима после окончания движения.
     * \param[in] horizontal Выполнять нормализацию по горизонтали.
     * \param[in] vertical Выполнять нормализацию по вертикали.
     */
    void normalizePosition(bool horizontal, bool vertical);
    void render(sf::RenderTarget &target, const sf::Transform &transform) const;

private:
    sf::Sprite m_sprite;

    /// Скорость, пиксли игрового мира в секунду.
    sf::Vector2f m_speed;
    sf::Vector2f m_movementLength;
};
