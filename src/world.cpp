#include "world.h"

#include <optional>
#include <set>

#include "initial_position.h"
#include "log.h"


namespace
{

constexpr int INITIAL_BOX_QUANTITY = 12;
/// Количество нижних рядов, на которых разрешено прыгать.
constexpr int ROWS_WITH_ALLOWED_JUMP = 3;

}


unsigned int World::m_boxTextureVariants = 1;


World::World()
{
    setup();
}


void World::init(
    sf::Texture textureBackground,
    sf::Texture texturePlayer,
    sf::Texture textureBox)
{
    m_textureBackground = std::move(textureBackground);
    m_background.setTexture(m_textureBackground);

    m_player.init(std::move(texturePlayer));

    m_textureBox = std::move(textureBox);
    m_boxTextureVariants = m_textureBox.getSize().x / BOX_SIZE;
}


void World::generateStartPosition(const std::optional<unsigned int> &position)
{
    clearBoxes();

    if (position.has_value() && position.value() < INITIAL_POSITIONS.size())
    {
        LOG_DEBUG("Set predefined initial position №" << position.value() << ".");
        generatePredefinedPosition(INITIAL_POSITIONS.at(position.value()));
    }
    else
    {
        generateRandomPosition();
    }
}


void World::update(const Duration &elapsed)
{
    updatePlayer(elapsed);

    for (auto &boxInfo : m_boxes)
    {
        Box &box = boxInfo.second;
        updateBox(box, elapsed);
    }
}


void World::requestMovePlayer(const Player::Direction direction)
{
    m_playerRequestedDirection = direction;
}


void World::requestStopPlayer()
{
    m_playerRequestedDirection = Player::Direction::None;
}


void World::movePlayer(const Player::Direction direction)
{
    const std::optional<Object::Coordinate> playerRow = m_player.row();
    const std::optional<Object::Coordinate> playerColumn = m_player.column();
    if (!canPlayerMove(playerRow, playerColumn, direction))
    {
        return;
    }

    // Смещение ящика после толкания.
    bool push = false;
    switch (direction)
    {
    case Player::Direction::Left:
        if (playerRow.value() == m_boxesLocations[playerColumn.value() - 1].size() - 1)
        {
            push = true;
            startMoveBox(playerRow.value(), playerColumn.value() - 1, Box::Direction::Left);
        }
        break;
    case Player::Direction::Right:
        if (playerRow.value() == m_boxesLocations[playerColumn.value() + 1].size() - 1)
        {
            push = true;
            startMoveBox(playerRow.value(), playerColumn.value() + 1, Box::Direction::Right);
        }
        break;
    default:
        break;
    }

    // Уточнение направления при прыжке вбок по диагонали.
    const Player::Direction secondDirection = playerNextDirection(
        direction,
        playerRow.value(),
        playerColumn.value());

    m_player.move(direction, push, secondDirection);
}


void World::render(sf::RenderTarget &target)
{
    target.draw(m_background);

    for (const auto &boxInfo : m_boxes)
    {
        const Box& box = boxInfo.second;
        box.render(target, m_transform);
    }

    m_player.render(target, m_transform);
}


void World::setup()
{
    std::random_device device;
    m_randomEngine = std::mt19937(device());

    // Переход в систему координат, у которой начало в левом нижнем углу
    // игровой области, ось X направлена вправо, ось Y направлена вверх.
    // https://www.sfml-dev.org/tutorials/2.6/graphics-transform.php
    m_transform.translate(BOTTOM_LEFT_CORNER);
    m_transform.scale(sf::Vector2f(1, -1));

    for (auto &column : m_boxesLocations)
    {
        column.reserve(BOXES_ROWS);
    }
}


void World::clearBoxes()
{
    for (auto &column : m_boxesLocations)
    {
        column.clear();
    }
}


void World::generateRandomPosition()
{
    constexpr Object::Coordinate MAX_BOXES_IN_COLUMN = 2;

    std::uniform_int_distribution<std::mt19937::result_type> distributionBoxStyle(
        0,
        m_boxTextureVariants - 1);
    std::uniform_int_distribution<std::mt19937::result_type> distributionColumn(
        0,
        BOXES_COLUMNS - 1);

    // Расстановка ящиков.
    std::set<Object::Coordinate> columnsUsed;
    for (int i = 0; i < INITIAL_BOX_QUANTITY; ++i)
    {
        Object::Coordinate column;
        do
        {
            column = distributionColumn(m_randomEngine);
        }
        while (
            // Проверка не превышен ли лимит высоты стопки:
            (m_boxesLocations[column].size() >= MAX_BOXES_IN_COLUMN) ||
            // Проверка не формирует ли последний ящик полный ряд:
            (i == INITIAL_BOX_QUANTITY - 1 &&
             columnsUsed.size() == BOXES_COLUMNS - 1 &&
             columnsUsed.find(column) == columnsUsed.cend()));
        columnsUsed.insert(column);

        const Object::Coordinate row = m_boxesLocations[column].size();
        addBox(distributionBoxStyle(m_randomEngine), row, column);
    }

    // Определение колонки игрока.
    for (Object::Coordinate i = 0; i < m_boxesLocations.size(); ++i)
    {
        if (m_boxesLocations[i].size() == MAX_BOXES_IN_COLUMN)
        {
            setPlayerColumn(i);
            break;
        }
    }
}


void World::generatePredefinedPosition(const InitialPosition &initialPosition)
{
    std::uniform_int_distribution<std::mt19937::result_type> distributionBoxStyle(
        0,
        m_boxTextureVariants - 1);

    for (Object::Coordinate column = 0; column < initialPosition.boxes.size(); ++column)
    {
        for (unsigned char row = 0; row < initialPosition.boxes[column]; ++row)
        {
            addBox(distributionBoxStyle(m_randomEngine), row, column);
        }
    }

    setPlayerColumn(initialPosition.playerColumn);
}


void World::addBox(unsigned int style, unsigned char row, unsigned char column)
{
    Box box(m_lastBoxId);
    box.init(m_textureBox);
    box.setStyle(style);
    box.setPosition(sf::Vector2f(column * BOX_SIZE, row * BOX_SIZE));

    m_boxes.emplace(m_lastBoxId, box);

    m_boxesLocations[column].push_back(m_lastBoxId);

    ++m_lastBoxId;
}


void World::setPlayerColumn(unsigned char column)
{
    const Object::Coordinate row = m_boxesLocations[column].size();
    const sf::Vector2f playerPosition(column * BOX_SIZE, row * BOX_SIZE);
    m_player.setPosition(playerPosition);
}


bool World::canPlayerMove(
    const std::optional<Object::Coordinate> &playerRow,
    const std::optional<Object::Coordinate> &playerColumn,
    const Player::Direction direction) const
{
    if (!playerColumn.has_value() || !playerRow.has_value())
    {
        return false;
    }

    const Object::Coordinate column = playerColumn.value();
    const Object::Coordinate row = playerRow.value();

// TODO: После прыжка вверх находясь в наивысшей точек прыжка можно пойти вбок если там есть ящик, на который можно сразу встать.
    switch (direction)
    {
    case Player::Direction::None:
        return false;
    case Player::Direction::Left:
        return canPlayerMoveLeftOrRight(row, column, true);
    case Player::Direction::Right:
        return canPlayerMoveLeftOrRight(row, column, false);
    case Player::Direction::UpLeft:
        return
            playerRow < ROWS_WITH_ALLOWED_JUMP &&
            column > 1 &&
            row == m_boxesLocations[column].size() &&
            row >= m_boxesLocations[column - 1].size() &&
            row + 1 >= m_boxesLocations[column - 2].size();
    case Player::Direction::UpRight:
        return
            playerRow < ROWS_WITH_ALLOWED_JUMP &&
            column < BOXES_COLUMNS - 2 &&
            row == m_boxesLocations[column].size() &&
            row >= m_boxesLocations[column + 1].size() &&
            row + 1 >= m_boxesLocations[column + 2].size();
    case Player::Direction::Up:
        return
            playerRow < ROWS_WITH_ALLOWED_JUMP &&
            row == m_boxesLocations[column].size();
    default:
        return false;
    }

    return false;
}


bool World::canPlayerMoveLeftOrRight(
    Object::Coordinate row,
    Object::Coordinate column,
    bool left) const
{
    const int k = left ? -1 : 1;

    if ((left && column <= 0) || (!left && column >= BOXES_COLUMNS - 1))
    {
        return false;
    }

    // Переход на соседнюю стопку без толкания ящика.
    // Игрок должен стоять на ящике, либо встать на ящик после перемещения.
    const Object::Coordinate nextColumnHeight = m_boxesLocations[column + k].size();
    const Object::Coordinate currentColumnHeight = m_boxesLocations[column].size();
    if (row == nextColumnHeight ||
        (row == currentColumnHeight && row > nextColumnHeight))
    {
        return true;
    }

    // Переход на соседнюю стопку с толканием ящика.
    // Должен быть запас в две стопки.
    // Высота соседней стопки должна быть не более чем на 1 больше, чем у текущей.
    // Высота следующей после соседней стопки должна быть не более, чем у текущей.
    return
        ((left && column >= 2) ||
        (!left && column <= BOXES_COLUMNS - 3)) &&
        row == nextColumnHeight - 1 &&
        row >= m_boxesLocations[column + 2 * k].size();
}


Player::Direction World::playerNextDirection(
    Player::Direction direction,
    Object::Coordinate row,
    Object::Coordinate column) const
{
    Player::Direction result = Player::Direction::None;
    if (direction == Player::Direction::Up)
    {
        result = Player::Direction::Down;
    }
    else if (direction == Player::Direction::UpLeft)
    {
        result = row + 1 == m_boxesLocations[column - 2].size()
            ? Player::Direction::Left
            : Player::Direction::DownLeft;
    }
    else if (direction == Player::Direction::UpRight)
    {
        result = row + 1 == m_boxesLocations[column + 2].size()
            ? Player::Direction::Right
            : Player::Direction::DownRight;
    }
    return result;
}


void World::updatePlayer(const Duration &elapsed)
{
    m_player.update(elapsed);

    if (m_playerRequestedDirection != Player::Direction::None)
    {
        movePlayer(m_playerRequestedDirection);
    }

    const std::optional<Object::Coordinate> playerColumn = m_player.column();
    if (!playerColumn.has_value() || m_player.isMoving())
    {
        return;
    }

    if (m_playerRequestedDirection == Player::Direction::None)
    {
        m_player.stop();
    }

    const float playerHeight = m_player.position().y;
    const float columnHeight = float(m_boxesLocations[playerColumn.value()].size()) * BOX_SIZE;
    if (playerHeight < columnHeight)
    {
        m_player.stopFalling();
    }
    if (playerHeight > columnHeight && !m_player.isMoving())
    {
        m_player.move(Player::Direction::Down);
    }
}


void World::updateBox(Box &box, const Duration &elapsed)
{
    box.update(elapsed);

    const std::optional<Object::Coordinate> boxColumn = box.column();
    if (!boxColumn.has_value())
    {
        return;
    }

    const Object::Coordinate column = boxColumn.value();
    const float boxHeight = box.position().y;
    const float columnHeight = float(m_boxesLocations[column].size()) * BOX_SIZE;
    if (boxHeight > columnHeight)
    {
        // Под ящиком есть пустота => ящик падает.
        box.move(Box::Direction::Down);
        return;
    }

    // Ящик стоит на стопке.
    // Переиндексация положения ящика после его остановки.
    if (box.direction() != Box::Direction::None)
    {
        box.stopFalling();
        m_boxesLocations[column].push_back(box.id());
        box.move(Box::Direction::None);
    }
}


void World::startMoveBox(
    Object::Coordinate row,
    Object::Coordinate column,
    Box::Direction direction)
{
    Box &box = this->box(row, column);
    box.move(direction);
    m_boxesLocations[column].erase(--m_boxesLocations[column].end());
}


void World::changeBoxColumn(
    Box::Id id,
    Object::Coordinate columnOld,
    Object::Coordinate columnNew)
{
    if (m_boxesLocations[columnOld].empty())
    {
        return;
    }
    if (m_boxesLocations[columnOld].back() != id)
    {
        return;
    }
    m_boxesLocations[columnOld].erase(--m_boxesLocations[columnOld].end());
    m_boxesLocations[columnNew].push_back(id);
}


Box& World::box(Object::Coordinate row, Object::Coordinate column)
{
    const Box::Id boxId = m_boxesLocations[column][row];
    return m_boxes[boxId];
}


Object::Coordinate World::boxesInBottomRow() const
{
    Object::Coordinate result = 0;
    for (const auto &column : m_boxesLocations)
    {
        result += column.size();
    }
    return result;
}
