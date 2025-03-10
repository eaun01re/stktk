#pragma once


#include <memory>

#include <SFML/Graphics.hpp>

#include "animation.h"
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

public:
    explicit Player() = default;
    virtual ~Player() = default;

    void init(const sf::Texture &texture) override;
    void update(const Duration &elapsed) override;
    void move(Direction direction, bool push = false, Direction nextDirection = Direction::None);

private:
    void setDirection(Direction direction, bool push);

private:
    std::unique_ptr<Animation> m_animation;
    Direction m_direction{ Direction::None };
    Direction m_nextDirection{ Direction::None };
};
