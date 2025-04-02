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

    enum Animations : AnimationId
    {
        Rest0,
        Rest1,
        Rest2,
        Rest3,
        Rest4,
        Rest5,
        Rest6,
        Rest7,
        Blow,
    };

public:
    explicit Box() = default;
    Box(const Box &box);
    virtual ~Box() = default;

    void init(const sf::Texture &texture) override;
    void update(const Duration &elapsed) override;
    Animations style() const noexcept;
    void move(Direction direction);
    Direction direction() const noexcept;
    void blow();
    bool isBlowed() const;

private:
    void setStyle(Animations style);

private:
    Animations m_style{ Animations::Rest0 };
    Direction m_direction{ Direction::None };
    std::optional<Duration> m_blowDuration;
};


using BoxPtr = std::shared_ptr<Box>;
