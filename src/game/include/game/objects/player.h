#pragma once


#include <functional>

#include <SFML/Graphics.hpp>

#include <game/objects/object.h>


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
    using MoveFinishedCallback = std::function<void(Direction)>;

public:
    static unsigned int height();

public:
    explicit Player(const MoveFinishedCallback &callback);
    virtual ~Player() = default;

    void update(const Duration &elapsed) override;
    void move(Direction direction, bool push = false);
    Direction direction() const noexcept;
    void idle();
    bool alive() const noexcept;
    void setAlive(bool alive);

protected:
    void moveFinished() override;

private:
    void init();
    void setDirection(Direction direction, bool push);
    const TextureSpriteIndices& animationByDirection(
        const sf::Vector2f &directions,
        bool push) const noexcept;

private:
    Direction m_direction{ Direction::None };
    bool m_lookLeft{ true };
    bool m_alive{ true };
    MoveFinishedCallback m_moveFinishedCallback;
};
