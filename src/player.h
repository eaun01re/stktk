#pragma once


#include <memory>

#include <SFML/Graphics.hpp>

#include "animation.h"
#include "box.h"


namespace
{

/// Скорость, пиксели в секунду.
constexpr float SPEED_HORIZONTAL = 2.0 * BOX_SIZE;
constexpr float SPEED_VECTICAL = 2.0 * BOX_SIZE / 1.5;

}

class Player final
{
public:
    enum class Direction
    {
        Left, ///< Смещение влево с возможным толканием.
        UpLeft, ///< Прыжок влево без толкания.
        Right, ///< Смещение вправо с возможным толканием.
        UpRight, ///< Прыжок вправо без толкания.
        Up ///< Прыжок вверх с возможным сбиванием падающего ящика.
    };

public:
    Player();

    void init(sf::Texture texture);
    void setPosition(const sf::Vector2f &position);
    sf::Vector2f position() const;
    const sf::Vector2f& speed() const;
    void move(Direction direction);
    void fall();
    bool isFalling() const;
    bool isRising() const;
    bool isMoving() const;
    void stopFalling();
    /*!
     * Нормализует положение, делая координаты кратными размеру ящика.
     * Нормализация необходима после окончания движения.
     * \param[in] horizontal Выполнять нормализацию по горизонтали.
     * \param[in] vertical Выполнять нормализацию по вертикали.
     */
    void normalizePosition(bool horizontal, bool vertical);
    void update(const Duration &elapsed);
    void render(sf::RenderTarget &target, const sf::Transform &transform) const;

private:
    sf::Sprite m_sprite;
    sf::Texture m_texture;
    std::unique_ptr<Animation> m_animation;

    /// Скорость, пиксли игрового мира в секунду.
    sf::Vector2f m_speed;
    sf::Vector2f m_movementLength;
};
