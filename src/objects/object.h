#pragma once


#include <memory>
#include <optional>

#include <SFML/Graphics.hpp>

#include "animation.h"
#include "clock.h"
#include "consts.h"


namespace
{

/// Скорость движения объектов, пиксели в секунду.
constexpr float SPEED = 1.5 * BOX_SIZE;

}


class Object
{
public:
    using Id = unsigned long;
    using Coordinate = unsigned int;

public:
    explicit Object();
    Object(const Object &object);
    virtual ~Object() = default;

    Id id() const noexcept;
    virtual void init(const sf::Texture &texture);
    void setPosition(const sf::Vector2f &position);
    const sf::Vector2f& position() const noexcept;
    const sf::Vector2f& speed() const noexcept;
    bool isFalling() const noexcept;
    bool isMoving() const;
    void stopFalling();
    std::optional<Coordinate> column() const;
    std::optional<Coordinate> row() const;
    /*!
     * Нормализует положение, делая координаты кратными размеру ящика.
     * Нормализация необходима после окончания движения.
     * \param[in] horizontal Выполнять нормализацию по горизонтали.
     * \param[in] vertical Выполнять нормализацию по вертикали.
     */
    void normalizePosition(bool horizontal, bool vertical);
    virtual void update(const Duration &elapsed);
    virtual void render(sf::RenderTarget &target, const sf::Transform &transform) const;

protected:
    sf::Vector2f move(const Duration &elapsed);
    virtual void moveFinished();

protected:
    const Id m_id;

    sf::Sprite m_sprite;
    sf::Texture m_texture;

    std::unique_ptr<Animation> m_animation;

    /// Скорость, пиксели игрового мира в секунду.
    sf::Vector2f m_speed;
    /// Текущее смещение с начала движения.
    sf::Vector2f m_offset;
    sf::Vector2f m_movementLength;

private:
    static Id m_lastId;
};


namespace
{

constexpr Object::Id NULL_ID = 0;

}
