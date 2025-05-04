#pragma once


#include <array>
#include <functional>
#include <optional>
#include <random>
#include <set>

#include <SFML/Graphics.hpp>

#include "screen.h"
#include "objects/box.h"
#include "objects/crane.h"
#include "objects/player.h"
#include "objects/figure.h"
#include "resource_loader.h"


namespace
{

constexpr std::size_t MAX_CRANES_QUANTITY = 5;

}


struct InitialPosition;


class World final : public Screen
{
public:
    explicit World();
    void start(
        std::uint8_t cranesQuantity,
        const std::optional<unsigned int> &positionIndex = std::nullopt);
    void update(const Duration &elapsed) override;
    void handleKeyPressed(const sf::Keyboard::Key key) override;
    void handleKeyReleased(const sf::Keyboard::Key key) override;
    void render(sf::RenderTarget &target) override;

    void requestMovePlayer(const Player::Direction direction);
    void requestStopPlayer();
    void togglePause();
    unsigned int score() const noexcept;

private:
    void setup();
    void resume();
    void pause();
    void clearObjects();
    /*!
     * Случайным образом расставляет ящики и определяет положение игрока.
     * Случайное стартовое положение должно соответствовать критериям:
     * 1. максимальное количество ящиков в стопке = 2;
     * 2. должна быть хотя бы одна стопка с более чем одним ящиком,
     *    иначе ящики образуют заполненный ряд;
     * 3. игрок находится на самой левой стопке из двух ящиков.
     * \sa MAX_BOXES_IN_COLUMN
     */
    void generateRandomPosition();
    void generatePredefinedPosition(const InitialPosition &initialPosition);
    /*!
     * Возвращает номер колонки, в которой располгается игрок в начале игры.
     * \return Первая слева колонка, имеющая максимальную допустимую
     * в начале игры высоту.
     * \sa MAX_BOXES_IN_COLUMN
     */
    Coordinate initialPlayerColumn() const;
    BoxPtr makeBox();
    BoxPtr addBox(Coordinate row, Coordinate column);
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
    void resetCrane(Crane &crane, float offsetLength = 0);
    void loadCrane(Crane &crane);
    void setPlayerColumn(Coordinate column);
    void movePlayer(const Player::Direction direction);
    bool canPlayerMove(
        const std::optional<Coordinate> &playerRow,
        const std::optional<Coordinate> &playerColumn,
        const Player::Direction direction,
        Object::Id &pushedBoxId) const;
    bool canPlayerMoveHorizontal(
        Coordinate row,
        Coordinate column,
        bool left,
        Object::Id &pushedBoxId) const;
    bool canPlayerMoveVertical(Coordinate row, Coordinate column) const;
    bool canPlayerMoveDiagonal(
        Coordinate row,
        Coordinate column,
        bool left) const;
    bool canPushBox(Box &box, Coordinate boxColumn, bool left) const;
    /*!
     * Возвращает следующее направление движения игрока, в котором он будет
     * двигаться после завершения текущего движения.
     * \param[in] direction Последнее направление движения игрока.
     * \param[in] row Текущий ряд игрока.
     * \param[in] column Текущая колонка игрока.
     * \return Если игрок достиг наивысшей точки прыжка по диагонали, то
     * направление, продолжающее этот прыжок.
     * В противном случае \c Player::Direction::None.
     */
    Player::Direction playerNextDirection(
        Player::Direction direction,
        Coordinate row,
        Coordinate column) const;
    void onPlayerMoveFinished(Player::Direction lastDirection);
    void onBoxMoveStarted(Object::Id boxId);
    void onBoxMoveFinished(Object::Id boxId);
    void updateActive(const Duration &elapsed);
    void updatePaused(const Duration &elapsed);
    void updatePlayer(const Duration &elapsed);
    void updateBoxes(const Duration &elapsed);
    /*!
     * Обновляет ящик.
     * \param[in] box Обновляемый ящик.
     * \param[in] elapsed Время, прошедшее с прошлого обновления.
     * \return \c true, если ящик следует немедленно удалить
     * (был сбит в воздухе), \c false - в противном случае.
     */
    bool updateBox(Box &box, const Duration &elapsed);
    void updateCrane(Crane &crane, const Duration &elapsed);
    void pushBox(Box &box, Box::Direction direction);
    bool boxHitsPlayer(const Box &box) const noexcept;
    bool canDropBox(Object::Id boxId, Coordinate column) const;
    void dropBox(Crane &crane);
    /*!
     * Запускает удаление нижнего ряда ящиков.
     * \return \c true, если нижний ряд уничтожен,
     * \c false - в противном случае.
     */
    bool blowBottomRow();
    /*!
     * \return \c true, если нижний ряд заполнен,
     * \c false - в противном случае.
     */
    bool bottomRowFilled() const;
    /*!
     * Возвращает высоту стопки, сложенной из неподвижных ящиков.
     * \param[in] column Номер столбца.
     * \return Количество неподвижных ящиков.
     */
    Coordinate columnHeight(Coordinate column) const noexcept;
    void renderCrane(Crane &crane, sf::RenderTarget &target) const;
    void playSound(ResourceLoader::SoundId id);
    void stop();

private:
    std::function<void(const Duration&)> m_updater;

    Player m_player;
    Player::Direction m_playerRequestedDirection{ Player::Direction::None };

    std::map<Object::Id, BoxPtr> m_boxes;
    // Индексация ящиков.
    /// Ящики, лежащие на полу.
    std::array<std::array<Object::Id, BOXES_ROWS>, BOXES_COLUMNS> m_boxesStatic;
    /// Ящики, находящиеся в движении, кроме удерживаемых кранами.
    std::set<Object::Id> m_boxesMoving;

    std::array<CranePtr, MAX_CRANES_QUANTITY> m_cranes;

    sf::Sprite m_background;
    sf::Sprite m_foreground;

    sf::Transform m_transform;

    /// Количество заработанных очков.
    unsigned int m_score{ 0 };
    /// Графические примитивы, используемые для отображения очков.
    std::vector<FigurePtr> m_scoreFigures;

    bool m_paused{ true };

    sf::Sound m_sound;

    mutable std::mt19937 m_randomEngine;
};
