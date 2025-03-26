#pragma once


#include <array>
#include <list>
#include <optional>
#include <random>

#include <SFML/Graphics.hpp>

#include "objects/box.h"
#include "objects/crane.h"
#include "objects/player.h"


namespace
{

/// Левый нижний угол области игрового мира, в которой происходит игра.
const sf::Vector2f BOTTOM_LEFT_CORNER(4, 60);

}


struct InitialPosition;


class World final
{
public:
    explicit World();
    void init();
    void reset(
        const std::uint8_t cranesQuantity,
        const std::optional<unsigned int> &positionIndex = std::nullopt);
    void update(const Duration &elapsed);
    void requestMovePlayer(const Player::Direction direction);
    void requestStopPlayer();
    void movePlayer(const Player::Direction direction);
    void render(sf::RenderTarget &target);

private:
    void setup();
    void clear();
    /*!
     * Случайным образом расставляет ящики и определяет положение игрока.
     * Случайное стартовое положение должно соответствовать критериям:
     * 1. максимальное количество ящиков в стопке = 2;
     * 2. должна быть хотя бы одна стопка с двумя ящиками (скорее всего),
     *    иначе в таком положении ящики образуют заполненный ряд;
     * 3. игрок находится на самой левой стопке из двух ящиков.
     */
    void generateRandomPosition();
    void generatePredefinedPosition(const InitialPosition &initialPosition);
    BoxPtr addBox();
    BoxPtr addBox(Object::Coordinate row, Object::Coordinate column);
    void addCranes(const std::uint8_t cranesQuantity);
    void addCrane();
    void resetCrane(Crane &crane);
    void loadCrane(Crane &crane);
    void setPlayerColumn(Object::Coordinate column);
    bool canPlayerMove(
        const std::optional<Object::Coordinate> &playerRow,
        const std::optional<Object::Coordinate> &playerColumn,
        const Player::Direction direction) const;
    bool canPlayerMoveLeftOrRight(
        Object::Coordinate row,
        Object::Coordinate column,
        bool left) const;
    Player::Direction playerNextDirection(
        Player::Direction direction,
        Object::Coordinate row,
        Object::Coordinate column) const;
    void updatePlayer(const Duration &elapsed);
    void updateBox(Box &box, const Duration &elapsed);
    void updateCrane(Crane &crane, const Duration &elapsed);
    void startMoveBox(
        Object::Coordinate row,
        Object::Coordinate column,
        Box::Direction direction);
    void checkBottomRow();
    bool bottomRowFilled() const;
    void removeBlowedBoxes();
    /*!
     * Возвращает высоту стопки, сложенной из неподвижных ящиков.
     * \param[in] column Номер столбца.
     * \return Количество неподвижных ящиков.
     */
    Object::Coordinate columnHeight(Object::Coordinate column) const noexcept;

private:
    Player m_player;
    Player::Direction m_playerRequestedDirection{ Player::Direction::None };

    std::map<Object::Id, BoxPtr> m_boxes;
    std::array<std::list<Object::Id>, BOXES_COLUMNS> m_boxesLocations;
    std::shared_ptr<const sf::Texture> m_textureBox;

    std::vector<CranePtr> m_cranes;
    std::shared_ptr<const sf::Texture> m_textureCrane;

    sf::Sprite m_background;
    sf::Sprite m_foreground;

    sf::Transform m_transform;

    mutable std::mt19937 m_randomEngine;
};
