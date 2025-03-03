#pragma once


#include <array>
#include <optional>
#include <random>

#include <SFML/Graphics.hpp>

#include "box.h"
#include "player.h"
#include "player.h"


namespace
{

/// Левый нижний угол области игрового мира, в которой происходит игра.
const sf::Vector2f BOTTOM_LEFT_CORNER(4, 60);
constexpr unsigned char BOXES_ROWS = 6; // FIXME: Выяснить!
constexpr unsigned char BOXES_COLUMNS = 12;

}


struct InitialPosition;

using BoxId = unsigned long;


class World final
{
public:
    World();

    void init(
        sf::Texture textureBackground,
        sf::Texture texturePlayer,
        sf::Texture textureBox);
    void generateStartPosition(const std::optional<unsigned int> &position = std::nullopt);

    void update(const Duration &elapsed);

//    sf::Vector2f generatePlayerStartPosition();

    // void limitPlayer(Player &player);

    void movePlayer(const Player::Direction direction);

    void render(sf::RenderTarget &target);

private:
    void setup();
    void clearBoxes();
    void generateRandomPosition();
    void generatePredefinedPosition(const InitialPosition &initialPosition);
    void addBox(unsigned int style, unsigned char row, unsigned char column);
    void setPlayerColumn(unsigned char column);
    bool canPlayerMove(
        const std::optional<unsigned int> &playerRow,
        const std::optional<unsigned int> &playerColumn,
        const Player::Direction direction) const;
    Box& box(unsigned int row, unsigned int column);

private:
    /// Количество вариантов текстуры ящика. Зависит от содержимого ресурсного файла.
    static unsigned int m_boxTextureVariants;

private:
    sf::Sprite m_background;
    sf::Texture m_textureBackground;
    sf::Texture m_textureBox;

    sf::Vector2u m_windowSize;

    // Переход в систему координат, у которой начало в левом нижнем углу
    // игровой области, ось X направлена вправо, ось Y направлена вверх.
    // https://www.sfml-dev.org/tutorials/2.6/graphics-transform.php
    // TODO: Сохранить глобально, чтобы не пересоздавать каждый раз.
    sf::Transform m_transform;

    Player m_player;

    std::array<std::vector<BoxId>, BOXES_COLUMNS> m_boxesLocations;
    std::map<BoxId, Box> m_boxes;
    BoxId m_lastBoxId{ 0 };

    mutable std::mt19937 m_randomEngine;
    mutable std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
};
