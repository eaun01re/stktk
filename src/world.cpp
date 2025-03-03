#include "world.h"

#include "initial_position.h"
#include "math.h"
#include <optional>


namespace
{

constexpr int INITIAL_BOX_QUANTITY = 12;

std::optional<unsigned int> playerColumn(const Player &player)
{
    const bool playerHasCertainColumn = normalizedPosition(player.position(), true, false) == player.position();
    const std::optional<unsigned int> result = playerHasCertainColumn
        ? std::optional<unsigned int>(std::round(player.position().x / BOX_SIZE))
        : std::nullopt;
    return result;
}

std::optional<unsigned int> playerRow(const Player &player)
{
    const bool playerHasCertainRow = normalizedPosition(player.position(), true, false) == player.position();
    const std::optional<unsigned int> result = playerHasCertainRow
        ? std::optional<unsigned int>(std::round(player.position().y / BOX_SIZE))
        : std::nullopt;
    return result;
}

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
        generatePredefinedPosition(INITIAL_POSITIONS.at(position.value()));
    }
    else
    {
        generateRandomPosition();
    }
}


void World::update(const Duration &elapsed)
{
    m_player.update(elapsed);

    const std::optional<unsigned int> playerColumn = ::playerColumn(m_player);
    if (playerColumn.has_value())
    {
        const float playerHeight = m_player.position().y;
        const float columnHeight = float(m_boxesLocations[playerColumn.value()].size()) * BOX_SIZE;
        if (playerHeight < columnHeight)
        {
            m_player.stopFalling();
        }
        if (playerHeight > columnHeight && !m_player.isRising())
        {
            m_player.fall();
        }
    }

    for (auto &boxInfo : m_boxes)
    {
        Box &box = boxInfo.second;
        box.update(elapsed);
    }
}


void World::movePlayer(const Player::Direction direction)
{
    const std::optional<unsigned int> playerRow = ::playerRow(m_player);
    const std::optional<unsigned int> playerColumn = ::playerColumn(m_player);
    if (!canPlayerMove(playerRow, playerColumn, direction))
    {
        return;
    }

    m_player.move(direction);

    // Смещение ящика после толкания.
    switch (direction)
    {
    case Player::Direction::Left:
        if (playerRow.value() == m_boxesLocations[playerColumn.value() - 1].size() - 1)
        {
            Box &boxToMove = box(playerRow.value(), playerColumn.value() - 1);
            boxToMove.move(Box::Direction::Left);
        }
        break;
    case Player::Direction::Right:
        break;
    default:
        break;
    }
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
    m_distribution = std::uniform_int_distribution<std::mt19937::result_type>(0, BOXES_COLUMNS - 1);

    // Переход в систему координат, у которой начало в левом нижнем углу
    // игровой области, ось X направлена вправо, ось Y направлена вверх.
    // https://www.sfml-dev.org/tutorials/2.6/graphics-transform.php
    m_transform.translate(BOTTOM_LEFT_CORNER);
    m_transform.scale({1, -1});

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
    std::uniform_int_distribution<std::mt19937::result_type> distributionBoxStyle(0, m_boxTextureVariants - 1);
    std::uniform_int_distribution<std::mt19937::result_type> distributionColumn(0, BOXES_COLUMNS - 1);

    for (int i = 0; i < INITIAL_BOX_QUANTITY; ++i)
    {
        const unsigned int column = distributionColumn(m_randomEngine);
        const unsigned int row = m_boxesLocations[column].size();
        addBox(distributionBoxStyle(m_randomEngine), row, column);
    }

    setPlayerColumn(distributionColumn(m_randomEngine));
}


void World::generatePredefinedPosition(const InitialPosition &initialPosition)
{
    std::uniform_int_distribution<std::mt19937::result_type> distributionBoxStyle(0, m_boxTextureVariants - 1);
    std::uniform_int_distribution<std::mt19937::result_type> distributionColumn(0, BOXES_COLUMNS - 1);

    for (unsigned int column = 0; column < initialPosition.boxes.size(); ++column)
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
    Box box;
    box.init(
        m_textureBox,
        style,
        sf::Vector2f(column * BOX_SIZE, row * BOX_SIZE));

    ++m_lastBoxId;
    // Box не хранит копию текстуры, т.к. при помещении его в контейнер
    // она ломается.
    m_boxes[m_lastBoxId] = box;

    m_boxesLocations[column].push_back(m_lastBoxId);
}


void World::setPlayerColumn(unsigned char column)
{
    const unsigned int row = m_boxesLocations[column].size();
    const sf::Vector2f playerPosition(column * BOX_SIZE, row * BOX_SIZE);
    m_player.setPosition(playerPosition);
}


bool World::canPlayerMove(
    const std::optional<unsigned int> &playerRow,
    const std::optional<unsigned int> &playerColumn,
    const Player::Direction direction) const
{
    // const std::optional<unsigned int> playerColumn = ::playerColumn(m_player);
    if (!playerColumn.has_value())
    {
        return false;
    }
    // const std::optional<unsigned int> playerRow = ::playerRow(m_player);
    if (!playerRow.has_value())
    {
        return false;
    }

    const unsigned int column = playerColumn.value();
    const unsigned int row = playerRow.value();

// TODO: После прыжка вверх находясь в наивысшей точек прыжка можно пойти вбок если там есть ящик, на который можно сразу встать.
    switch (direction)
    {
    case Player::Direction::Left:
        return ((column > 0 && row >= m_boxesLocations[column - 1].size()) ||
                (column > 1 && row == m_boxesLocations[column - 1].size() - 1 && row >= m_boxesLocations[column - 2].size()));
    case Player::Direction::UpLeft:
        return (column > 1 &&
                row >= m_boxesLocations[column - 1].size() &&
                row + 1 >= m_boxesLocations[column - 2].size());
    case Player::Direction::Right:
        return ((column < BOXES_COLUMNS && row >= m_boxesLocations[column + 1].size()) ||
                (column < BOXES_COLUMNS - 1 && row == m_boxesLocations[column + 1].size() - 1 && row >= m_boxesLocations[column + 2].size()));
    case Player::Direction::UpRight:
        return (column < BOXES_COLUMNS - 1 &&
                row >= m_boxesLocations[column + 1].size() &&
                row + 1 >= m_boxesLocations[column + 2].size());
    case Player::Direction::Up:
        return true;
    }

    return false;
}


Box& World::box(unsigned int row, unsigned int column)
{
    const BoxId boxId = m_boxesLocations[column][row];
    return m_boxes[boxId];
}
