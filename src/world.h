#pragma once


#include <array>
#include <optional>
#include <random>

#include <SFML/Graphics.hpp>

#include "objects/box.h"
#include "objects/player.h"


namespace
{

/// Левый нижний угол области игрового мира, в которой происходит игра.
const sf::Vector2f BOTTOM_LEFT_CORNER(4, 60);
constexpr unsigned char BOXES_ROWS = 5;
constexpr unsigned char BOXES_COLUMNS = 12;

}


struct InitialPosition;


class World final
{
public:
    explicit World();

    void init(
        sf::Texture textureBackground,
        sf::Texture texturePlayer,
        sf::Texture textureBox);
    void generateStartPosition(const std::optional<unsigned int> &position = std::nullopt);

    void update(const Duration &elapsed);

//    sf::Vector2f generatePlayerStartPosition();

    // void limitPlayer(Player &player);

    void requestMovePlayer(const Player::Direction direction);
    void requestStopPlayer();
    void movePlayer(const Player::Direction direction);

    void render(sf::RenderTarget &target);

private:
    void setup();
    void clearBoxes();
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
    void addBox(unsigned int style, unsigned char row, unsigned char column);
    void setPlayerColumn(unsigned char column);
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
    void startMoveBox(
        Object::Coordinate row,
        Object::Coordinate column,
        Box::Direction direction);
    void changeBoxColumn(
        Box::Id id,
        Object::Coordinate columnOld,
        Object::Coordinate columnNew);
    Box& box(Object::Coordinate row, Object::Coordinate column);
    Object::Coordinate boxesInBottomRow() const;

private:
    /// Количество вариантов текстуры ящика. Зависит от содержимого ресурсного файла.
    static unsigned int m_boxTextureVariants;

private:
    sf::Sprite m_background;
    sf::Texture m_textureBackground;
    sf::Transform m_transform;

    sf::Texture m_textureBox;

    Player m_player;
    Player::Direction m_playerRequestedDirection{ Player::Direction::None };

    std::array<std::vector<Box::Id>, BOXES_COLUMNS> m_boxesLocations;
    std::map<Box::Id, Box> m_boxes;
    Box::Id m_lastBoxId{ 0 };

    mutable std::mt19937 m_randomEngine;
};
