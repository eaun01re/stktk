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

constexpr std::size_t MAX_CRANES_QUANTITY = 5;

}


struct InitialPosition;


class World final
{
public:
    explicit World();
    void init();
    void reset(
        std::uint8_t cranesQuantity,
        const std::optional<unsigned int> &positionIndex = std::nullopt);
    void update(const Duration &elapsed);
    void requestMovePlayer(const Player::Direction direction);
    void requestStopPlayer();
    void movePlayer(const Player::Direction direction);
    void render(sf::RenderTarget &target);

private:
    void setup();
    void clearObjects();
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
    void addCranes(uint8_t cranesQuantity);
    /*!
     * Логика управления кранами, выработанная экспериментально.
     * Может отличаться логики оригинальной игры.
     * В игре предусмотрено фиксированное количество мест или "ячеек",
     * в которые можно установить кран. Расстояние между двумя соседними
     * ячейками также фиксировано. Это расстояние и количество ячеек
     * подобрано так, чтобы на экране одновременно помещались все краны,
     * кроме последнего, который был бы виден частично.
     * Ячейки образуют кольцо и постоянно циркулируют по кругу. Как только
     * ячейка покидает область видимости (фактически проверяется видимость
     * крана в ней) она начинает новый круг со стартового положения.
     * В зависимости от направления движения - вправо или влево - этим
     * положением является место за левым или правым краем экрана
     * соответственно, чтобы там целиком умещался кран и не был при этом
     * виден. Направление движения на старте выбирается случайным образом.
     * В начале игры кольцо из ячеек повернуто так, что в стартовом положении
     * находится первая ячейка.
     * Краны добавляются в игру в её начале, а также когда в процессе игры
     * уничтожается заполненный нижний ряд ящиков и при этом есть свободные
     * ячейки. Первый кран занимает первую ячейку. Для каждого следующего
     * добавляемого крана выбирается та свободная ячейка, которой следующей
     * по очереди начинать новый круг. Если эта ячейка хотя бы частично видна
     * на момент добавления крана (исключением является случай, когда она
     * находится в стартовом положении), то кран вводится в игру как бы на
     * следующем круге циркулирования этой ячейки, чтобы не возникать внезапно
     * посреди экрана. Это достигается путём добавления крану смещения длиной
     * L = C * I, где C - количество ячеек, I - интервал между соседними
     * ячейками.
     */
    void addCrane();
    std::size_t cranesQuantity() const noexcept;
    CranePtr makeCrane();
    void resetCrane(Crane &crane, float offsetLength = 0);
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
    /*!
     * Запускает удаление нижнего ряда ящиков, если он заполнен.
     * \sa bottomRowFilled.
     * \return \c true, если нижний ряд заполнен, \c false - в противном случае.
     */
    bool blowBottomRow();
    /*!
     * \return \c true, если нижний ряд заполнен, \c false - в противном случае.
     */
    bool bottomRowFilled() const;
    /*!
     * \return \c true, если нижний ряд уничтожен, \c false - в противном случае.
     */
    bool removeBlowedRow();
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

    std::array<CranePtr, MAX_CRANES_QUANTITY> m_cranes;
    std::shared_ptr<const sf::Texture> m_textureCrane;

    sf::Sprite m_background;
    sf::Sprite m_foreground;

    sf::Transform m_transform;

    mutable std::mt19937 m_randomEngine;
};
