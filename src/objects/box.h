#pragma once


#include <memory>

#include <SFML/Graphics.hpp>

#include "animation.h"
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
    explicit Box(Id id = 0);
    Box(const Box &box);
    virtual ~Box() = default;

    void init(const sf::Texture &texture) override;
    void update(const Duration &elapsed) override;
    Id id() const noexcept;
    Animations style() const noexcept;
    void move(Direction direction);
    Direction direction() const noexcept;
    void blow();
    bool isBlowed() const;

private:
    void setStyle(Animations style);

private:
    const Id m_id;
    Animations m_style{ Animations::Rest0 };
    Direction m_direction{ Direction::None };

    std::optional<TimePoint> m_blowStart;
    std::unique_ptr<Animation> m_animation;
};
