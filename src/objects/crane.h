#pragma once


#include <SFML/Graphics.hpp>

#include "objects/object.h"


namespace
{

/// Высота, на которой расположен кран (нижняя граница текстуры).
constexpr int CRANE_VERTICAL_POSITION = 44;

/// Смещение положения удерживаемого ящика относительно положения крана.
const sf::Vector2f BOX_OFFSET(4, -3);

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
    void move(bool left);
    bool isLeft() const noexcept;
    Id boxId() const noexcept;
    void setBoxId(Id id);
    Coordinate dropColumn() const noexcept;
    void setDropColumn(Coordinate column);
    void drop();

private:
    bool m_left{ true };
    Id m_boxId{ NULL_ID };
    Coordinate m_dropColumn;
};

using CranePtr = std::shared_ptr<Crane>;
