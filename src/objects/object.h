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


using Coordinate = unsigned int;


struct Coordinates
{
    std::optional<Coordinate> row;
    std::optional<Coordinate> column;
};


class Object : public sf::Sprite
{
public:
    using Id = unsigned long;

public:
    explicit Object();
    Object(const Object &object);
    virtual ~Object() = default;

    virtual void update(const Duration &elapsed);

    Id id() const noexcept;
    const sf::Vector2f& speed() const noexcept;
    bool isFalling() const noexcept;
    bool isMoving() const;
    void stopFalling();
    Coordinates coordinates() const;
    std::optional<Coordinate> column() const;
    std::optional<Coordinate> row() const;
    /*!
     * Нормализует положение, делая координаты кратными размеру ящика.
     * Нормализация необходима после окончания движения.
     * \param[in] horizontal Выполнять нормализацию по горизонтали.
     * \param[in] vertical Выполнять нормализацию по вертикали.
     */
    void normalizePosition(bool horizontal, bool vertical);
    void setAnimation(const AnimationOriented &animation);
    void setAnimationSequence(const std::vector<AnimationOriented> &ids);

protected:
    void move(const Duration &elapsed);
    virtual void moveFinished();

protected:
    const Id m_id;

    std::unique_ptr<Animator> m_animator;
    std::vector<AnimationOriented> m_animationIds;
    std::size_t m_currentAnimationId{ 0 };

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
