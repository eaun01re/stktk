#pragma once


#include <SFML/Graphics.hpp>

#include "objects/object.h"


class Player final : public Object
{
public:
    enum Direction : std::uint8_t
    {
        None = 0,
        Left = 1,
        Right = 1 << 1,
        Up = 1 << 2,
        Down = 1 << 3,

        UpLeft = Up | Left,
        UpRight = Up | Right,
        DownLeft = Down | Left,
        DownRight = Down | Right
    };

    enum Animations : AnimationId
    {
        Idle,
        Walk,
        Push,
        Jump,
        Dying,
        Dead
    };

public:
    static unsigned int height();

public:
    explicit Player() = default;
    virtual ~Player() = default;

    void init(const sf::Texture &texture) override;
    void update(const Duration &elapsed) override;
    void move(
        Direction direction,
        bool push = false,
        Direction nextDirection = Direction::None);
    Direction direction() const noexcept;
    void stop();
    void die();

protected:
    void moveFinished() override;

private:
    void setDirection(Direction direction, bool push);

private:
    Direction m_direction{ Direction::None };
    Direction m_nextDirection{ Direction::None };
    bool m_lookLeft{ true };
};
