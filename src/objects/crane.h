#pragma once


#include <SFML/Graphics.hpp>

#include "objects/object.h"


namespace
{

/// Высота, на которой расположен кран (нижняя граница текстуры).
constexpr int CRANE_VERTICAL_POSITION = 44;

}


class Crane final : public Object
{
public:
    enum State : AnimationId
    {
        Holding,
        Open
    };

public:
    explicit Crane() = default;
    Crane(const Crane &crane);
    virtual ~Crane() = default;

    void init(const sf::Texture &texture) override;
    void update(const Duration &elapsed) override;
    float width() const noexcept;
    void setDirection(bool left);
    bool isLeft() const noexcept;

private:
    bool m_left{ true };
};
