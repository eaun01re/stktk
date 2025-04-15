#pragma once


#include <SFML/Graphics.hpp>

#include "objects/object.h"


class Box final : public Object
{
public:
    enum class Direction
    {
        None,
        Left,
        Right,
        Down
    };

public:
    explicit Box();
    virtual ~Box() = default;

    void update(const Duration &elapsed) override;
    void move(Direction direction);
    Direction direction() const noexcept;
    bool blow();
    bool isBlowing() const noexcept;
    bool isBlowed() const noexcept;

private:
    void init();

private:
    Direction m_direction{ Direction::None };
    std::optional<Duration> m_blowDuration;
};


using BoxPtr = std::shared_ptr<Box>;
