#pragma once


#include <SFML/Graphics.hpp>

#include <game/graphics/objects/object.h>


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
    explicit Crane();
    virtual ~Crane() = default;

    void update(const Duration &elapsed) override;

    int width() const noexcept;

    /*!
     * Перемещает кран в начальное положение. Задает направление и расстояние,
     * которое кран должен преодолеть.
     * \param[in] position Начальное положение крана.
     * \param[in] left Направление движения.
     * \param[in] movementLength Расстояние, которое необходимо преодолеть.
     */
    void reset(const sf::Vector2f &position, bool left, float movementLength);
    void stop();

    bool isLeft() const noexcept;

    Id boxId() const noexcept;
    void load(Id boxId);
    bool isLoaded() const noexcept;

    Coordinate dropColumn() const noexcept;
    void setDropColumn(Coordinate column);
    void drop();

    /*!
     * \return \c true, если кран проехал целый круг,
     * \c false - в противном случае.
     */
    bool readyToReset() const noexcept;

protected:
    void moveFinished() override;

private:
    void init();
    void setDirection(bool left, float movementLength);

private:
    bool m_left{ true };
    bool m_readyToReset{ false };
    Id m_boxId{ NULL_ID };
    Coordinate m_dropColumn;
};

using CranePtr = std::shared_ptr<Crane>;
