#pragma once


#include <SFML/Graphics.hpp>

#include "objects/object.h"


class Box final : public Object
{
public:
    using Id = unsigned long;

public:
    enum class Direction
    {
        None,
        Left,
        Right,
        Down
    };

public:
    explicit Box(Id id = 0);
    Box(const Box &box);
    virtual ~Box() = default;

    void init(const sf::Texture &texture) override;
    void update(const Duration &elapsed) override;
    Id id() const;
    unsigned int style() const noexcept;
    void setStyle(unsigned int style);
    void move(Direction direction);
    Direction direction() const noexcept;

private:
    Id m_id{ 0 };
    unsigned int m_style{ 0 };
    Direction m_direction{ Direction::None };
};
