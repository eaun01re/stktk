#include "world.h"

#include <optional>
#include <set>

#include "initial_position.h"
#include "log.h"
#include "math.h"
#include "resource_loader.h"
#include "objects/hourglass.h"
#include "objects/number.h"


namespace
{

constexpr sf::Keyboard::Key KEY_PAUSE = sf::Keyboard::Key::Num0;
constexpr sf::Keyboard::Key KEY_RESTART = sf::Keyboard::Key::Space;

/// Количество ящиков в начале игры.
constexpr int INITIAL_BOX_QUANTITY = 12;
/// Максимальная высота стопки ящиков в начале игры.
constexpr Coordinate MAX_BOXES_IN_COLUMN = 2;
/// Количество нижних рядов, на которых разрешено прыгать.
constexpr int ROWS_WITH_ALLOWED_JUMP = 3;
/// Максимальное количество кранов в начале игры.
constexpr std::uint8_t MAX_INITIAL_CRANES_QUANTITY = 3;
/// Расстояние в пикселях игрового мира между соседними ячейками для кранов.
constexpr unsigned int CRANE_INTERVAL = 22;
/// Интервал времени между появлениями кранов в начале игры.
/// Левый нижний угол области игрового мира, в которой происходит игра.
const sf::Vector2f BOTTOM_LEFT_CORNER(4, 60);
/// Ширина области, в которой видны движущиеся объекты игры.
const unsigned int GAME_REGION_WIDTH = SCREEN_SIZE.x - BOTTOM_LEFT_CORNER.x;
/// Очки, начисляемые за сборс краном ящика.
const unsigned int DROP_BOX_SCORE = 2;
/// Множитель количества очков, начиляемых за заполнение нижнего ряда.
const unsigned int BLOW_BOTTOM_ROW_SCORE_MULTIPLIER = 10;
/// Положение окна, отображающего текущий счёт игры.
const sf::Vector2f SCORE_POSITION_PAUSED(22, 9);
/// Положение окна, отображающего итоговый счёт игры.
const sf::Vector2f SCORE_POSITION_STOPPED(22, 19);
/// Положение песочных часов, отображаемых во время паузы.
const sf::Vector2f HOURGLASS_POSITION(40, 27);

sf::Vector2f craneStartPosition(
    bool left,
    unsigned int craneWidth,
    float offset)
{
    const sf::Vector2f position(
        left
            ? GAME_REGION_WIDTH + offset
            : -float(craneWidth) - offset,
        CRANE_VERTICAL_POSITION);
    return position;
}

Player::Direction directionByKey(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::Key::Left:
    case sf::Keyboard::Key::A:
        return Player::Direction::Left;
    case sf::Keyboard::Key::Right:
    case sf::Keyboard::Key::D:
        return Player::Direction::Right;
    case sf::Keyboard::Key::Up:
    case sf::Keyboard::Key::W:
        return Player::Direction::Up;
    case sf::Keyboard::Key::Q:
        return Player::Direction::UpLeft;
    case sf::Keyboard::Key::E:
        return Player::Direction::UpRight;
    default:
        return Player::Direction::None;
    }
}

std::shared_ptr<Hourglass> makeHourglass()
{
    std::shared_ptr<Hourglass> hourglass = std::make_shared<Hourglass>();
    hourglass->setPosition(HOURGLASS_POSITION);
    return hourglass;
}

}


World::World()
{
    setup();
}


void World::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();

    m_background.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Background));

    m_foreground.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Foreground));
    m_foreground.setColor(BACKGROUND_COLOR);
}


void World::start(
    std::uint8_t cranesQuantity,
    const std::optional<unsigned int> &positionIndex)
{
    // Удаление старых объектов.
    clearObjects();
    m_scoreFigures.clear();

    // Инициализация новых объектов.
    Coordinate playerColumn = 0;
    if (positionIndex.has_value() && positionIndex.value() < INITIAL_POSITIONS.size())
    {
        LOG_DEBUG("Set predefined initial position №" << positionIndex.value() << ".");
        const InitialPosition &initialPosition =
            INITIAL_POSITIONS.at(positionIndex.value());
        generatePredefinedPosition(initialPosition);
        playerColumn = initialPosition.playerColumn;
    }
    else
    {
        generateRandomPosition();
        playerColumn = initialPlayerColumn();
    }

    setPlayerColumn(playerColumn);
    m_player.setAlive(true);

    addCranes(cranesQuantity);
    m_score = 0;
    m_paused = false;
    resume();
}


void World::update(const Duration &elapsed)
{
    m_updater(elapsed);
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
    const Coordinates playerCoordinates = m_player.coordinates();
    if (!canPlayerMove(
            playerCoordinates.row,
            playerCoordinates.column,
            direction))
    {
        return;
    }

    // Смещение ящика после толкания.
    bool push = false;
    switch (direction)
    {
    case Player::Direction::Left:
        if (playerCoordinates.row.value() == columnHeight(playerCoordinates.column.value() - 1) - 1)
        {
            push = true;
            pushBox(
                playerCoordinates.row.value(),
                playerCoordinates.column.value() - 1,
                Box::Direction::Left);
        }
        break;
    case Player::Direction::Right:
        if (playerCoordinates.row.value() == columnHeight(playerCoordinates.column.value() + 1) - 1)
        {
            push = true;
            pushBox(
                playerCoordinates.row.value(),
                playerCoordinates.column.value() + 1,
                Box::Direction::Right);
        }
        break;
    default:
        break;
    }

    // Уточнение направления при прыжке вбок по диагонали.
    const Player::Direction secondDirection = playerNextDirection(
        direction,
        playerCoordinates.row.value(),
        playerCoordinates.column.value());

    m_player.move(direction, push, secondDirection);
}


void World::render(sf::RenderTarget &target)
{
    target.draw(m_background);

    // Отображение кранов и их ящиков.
    for (const CranePtr &crane : m_cranes)
    {
        if (crane != nullptr)
        {
            renderCrane(*crane, target);
        }
    }

    // Отображение свободных ящиков.
    for (auto &column : m_boxesStatic)
    {
        for (const Object::Id boxId : column)
        {
            if (boxId == NULL_ID)
            {
                break;
            }
            const BoxPtr &box = m_boxes.at(boxId);
            target.draw(*box, m_transform);
        }
    }
    for (const Object::Id boxId : m_boxesMoving)
    {
        const BoxPtr &box = m_boxes.at(boxId);
        target.draw(*box, m_transform);
    }

    // Отображение игрока.
    target.draw(m_player, m_transform);

    // Отображение счёта.
    for (const auto &figure : m_scoreFigures)
    {
        figure->render(target, m_transform);
    }

    // Отображение переднего плана.
    target.draw(m_foreground);
}


void World::handleKeyPressed(sf::Keyboard::Key key)
{
    if (!m_player.alive())
    {
        if (key == KEY_RESTART)
        {
            start(1);
        }
        return;
    }

    if (key == KEY_PAUSE)
    {
        togglePause();
        return;
    }

    const Player::Direction requestedDirection = directionByKey(key);
    if (requestedDirection == Player::Direction::None)
    {
        return;
    }
    requestMovePlayer(requestedDirection);
}


void World::handleKeyReleased(sf::Keyboard::Key key)
{
    const Player::Direction requestedDirection = directionByKey(key);
    if (requestedDirection == Player::Direction::None)
    {
        return;
    }
    requestStopPlayer();
}


void World::togglePause()
{
    m_paused = !m_paused;
    m_paused ? pause() : resume();
}


unsigned int World::score() const noexcept
{
    return m_score;
}


void World::setup()
{
    std::random_device device;
    m_randomEngine = std::mt19937(device());

    m_transform = sf::Transform();
    // Переход в систему координат, у которой начало в левом нижнем углу
    // игровой области, ось X направлена вправо, ось Y направлена вверх.
    // https://www.sfml-dev.org/tutorials/2.6/graphics-transform.php
    m_transform.translate(BOTTOM_LEFT_CORNER);
    m_transform.scale(sf::Vector2f(1, -1));
}


void World::resume()
{
    m_updater = std::bind(&World::updateActive, this, std::placeholders::_1);
    m_scoreFigures.clear();
}


void World::pause()
{
    m_updater = std::bind(&World::updatePaused, this, std::placeholders::_1);
    NumberPtr number = std::make_shared<Number>(m_score);
    number->setPosition(SCORE_POSITION_PAUSED);
    m_scoreFigures.push_back(number);
    m_scoreFigures.push_back(makeHourglass());
}


void World::clearObjects()
{
    m_boxes.clear();

    for (auto &crane : m_cranes)
    {
        crane.reset();
    }

    // Удаление индексов.
    for (auto &column : m_boxesStatic)
    {
        for (auto &boxId : column)
        {
            boxId = NULL_ID;
        }
    }
    m_boxesMoving.clear();
}


void World::generateRandomPosition()
{
    std::uniform_int_distribution<std::mt19937::result_type> distributionColumn(
        0,
        m_boxesStatic.size() - 1);

    // Расстановка ящиков.
    std::set<Coordinate> columnsUsed;
    for (int i = 0; i < INITIAL_BOX_QUANTITY; ++i)
    {
        Coordinate column;
        do
        {
            column = distributionColumn(m_randomEngine);
        }
        while (
            // Проверка не превышен ли лимит высоты стопки:
            (columnHeight(column) >= MAX_BOXES_IN_COLUMN) ||
            // Проверка не формирует ли последний ящик полный ряд:
            (i == INITIAL_BOX_QUANTITY - 1 &&
             columnsUsed.size() == BOXES_COLUMNS - 1 &&
             columnsUsed.find(column) == columnsUsed.cend()));
        columnsUsed.insert(column);

        const Coordinate row = columnHeight(column);
        addBox(row, column);
    }
}


Coordinate World::initialPlayerColumn() const
{
    for (Coordinate i = 0; i < m_boxesStatic.size(); ++i)
    {
        if (m_boxesStatic[i].size() == MAX_BOXES_IN_COLUMN)
        {
            return i;
        }
    }
    return 0;
}


void World::generatePredefinedPosition(const InitialPosition &initialPosition)
{
    for (Coordinate column = 0; column < initialPosition.boxes.size(); ++column)
    {
        for (Coordinate row = 0; row < initialPosition.boxes[column]; ++row)
        {
            addBox(row, column);
        }
    }

    setPlayerColumn(initialPosition.playerColumn);
}


BoxPtr World::loadCrane()
{
    BoxPtr box = std::make_shared<Box>();
    m_boxes.emplace(box->id(), box);
    return box;
}


BoxPtr World::addBox(Coordinate row, Coordinate column)
{
    BoxPtr box = std::make_shared<Box>();
    box->setPosition(sf::Vector2f(column * BOX_SIZE, row * BOX_SIZE));
    m_boxesStatic[column][row] = box->id();
    m_boxes.emplace(box->id(), box);
    return box;
}


void World::addCranes(std::uint8_t cranesQuantity)
{
    // Ограничение стартового количества кранов.
    cranesQuantity = std::clamp(
        cranesQuantity,
        std::uint8_t(1),
        MAX_INITIAL_CRANES_QUANTITY);
    for (unsigned int i = 0; i < cranesQuantity; ++i)
    {
        addCrane();
    }
}


void World::addCrane()
{
    const std::size_t cranesQuantity = this->cranesQuantity();
    if (cranesQuantity >= MAX_CRANES_QUANTITY)
    {
        // Достигнуто максимальное количество кранов.
        return;
    }

    CranePtr crane = std::make_shared<Crane>();
    // Первый кран добавляется в начале игры.
    // Он имеет нулевые индекс и смещение.
    unsigned int craneIndex = 0;
    float craneOffset = 0;
    if (cranesQuantity != 0)
    {
        // Поиск положения для второго и последующий кранов.
        const CranePtr &firstCrane = m_cranes.front();
        const sf::Vector2f craneStartPosition = ::craneStartPosition(
            firstCrane->isLeft(),
            firstCrane->width(),
            0);
        const float firstCraneOffset =
            std::abs(firstCrane->getPosition().x - craneStartPosition.x);
        float offset = 0;
        for (unsigned int i = 1; i < m_cranes.size(); ++i)
        {
            if (m_cranes.at(i) != nullptr)
            {
                // Данная ячейка занята другим краном.
                continue;
            }
            offset = firstCraneOffset - i * CRANE_INTERVAL;
            craneIndex = i;
            if (offset <= 0)
            {
                // Ячейча частично видна.
                break;
            }
        }
        if (craneIndex == m_cranes.size() - 1 && offset > 0)
        {
            offset -= m_cranes.size() * CRANE_INTERVAL;
        }
        craneOffset = offset;
    }

    resetCrane(*crane.get(), std::abs(craneOffset));
    m_cranes[craneIndex] = crane;
}


std::size_t World::cranesQuantity() const noexcept
{
    std::size_t cranesQuantity = 0;
    for (unsigned int i = 0; i < m_cranes.size(); ++i)
    {
        if (m_cranes.at(i) != nullptr)
        {
            ++cranesQuantity;
        }
    }
    return cranesQuantity;
}


void World::resetCrane(Crane &crane, float offsetLength)
{
    std::uniform_int_distribution<std::mt19937::result_type> distributionDirection(0, 1);
    const bool left = distributionDirection(m_randomEngine) == 0;
    const sf::Vector2f position =
        craneStartPosition(left, crane.width(), offsetLength);
    const float movementLength =
        offsetLength + crane.width() + GAME_REGION_WIDTH;
    crane.reset(position, left, movementLength);
    std::uniform_int_distribution<std::mt19937::result_type> distributionColumn(
        0,
        m_boxesStatic.size() - 1);
    crane.setDropColumn(distributionColumn(m_randomEngine));

    if (!crane.isLoaded())
    {
        const BoxPtr box = loadCrane();
        crane.load(box->id());
    }
    BoxPtr &box = m_boxes[crane.boxId()];
    box->setPosition(sum(crane.getPosition(), BOX_OFFSET));
}


void World::setPlayerColumn(Coordinate column)
{
    const Coordinate row = columnHeight(column);
    const sf::Vector2f playerPosition(column * BOX_SIZE, row * BOX_SIZE);
    m_player.setPosition(playerPosition);
}


void World::updateActive(const Duration &elapsed)
{
    // Обновление игрока.
    updatePlayer(elapsed);

    // Обновление кранов.
    for (CranePtr &crane : m_cranes)
    {
        if (crane != nullptr)
        {
            updateCrane(*crane.get(), elapsed);
        }
    }

    updateBoxes(elapsed);

    // Проверка нижнего ряда.
    if (bottomRowFilled())
    {
        if (blowBottomRow())
        {
            addCrane();
        }
    }
}


void World::updatePaused(const Duration&)
{
}


bool World::canPlayerMove(
    const std::optional<Coordinate> &playerRow,
    const std::optional<Coordinate> &playerColumn,
    const Player::Direction direction) const
{
    if (!playerColumn.has_value() || !playerRow.has_value())
    {
        return false;
    }

    const Coordinate column = playerColumn.value();
    const Coordinate row = playerRow.value();

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
            row == columnHeight(column) &&
            row >= columnHeight(column - 1) &&
            row + 1 >= columnHeight(column - 2);
    case Player::Direction::UpRight:
        return
            playerRow < ROWS_WITH_ALLOWED_JUMP &&
            column < BOXES_COLUMNS - 2 &&
            row == columnHeight(column) &&
            row >= columnHeight(column + 1) &&
            row + 1 >= columnHeight(column + 2);
    case Player::Direction::Up:
        return
            playerRow < ROWS_WITH_ALLOWED_JUMP &&
            row == columnHeight(column);
    default:
        return false;
    }

    return false;
}


bool World::canPlayerMoveLeftOrRight(
    Coordinate row,
    Coordinate column,
    bool left) const
{
    const int k = left ? -1 : 1;

    if ((left && column <= 0) || (!left && column >= BOXES_COLUMNS - 1))
    {
        return false;
    }

    // Переход на соседнюю стопку без толкания ящика.
    // Игрок должен стоять на ящике, либо встать на ящик после перемещения.
    const Coordinate nextColumnHeight = columnHeight(column + k);
    const Coordinate currentColumnHeight = columnHeight(column);
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
        row >= columnHeight(column + 2 * k);
}


Player::Direction World::playerNextDirection(
    Player::Direction direction,
    Coordinate row,
    Coordinate column) const
{
    Player::Direction result = Player::Direction::None;
    if (direction == Player::Direction::UpLeft)
    {
        result = row + 1 == columnHeight(column - 2)
            ? Player::Direction::Left
            : Player::Direction::DownLeft;
    }
    else if (direction == Player::Direction::UpRight)
    {
        result = row + 1 == columnHeight(column + 2)
            ? Player::Direction::Right
            : Player::Direction::DownRight;
    }
    return result;
}


void World::updatePlayer(const Duration &elapsed)
{
    m_player.update(elapsed);

    if (m_playerRequestedDirection != Player::Direction::None
        && (m_player.direction() == Player::Direction::None || m_player.isFalling()))
    {
        movePlayer(m_playerRequestedDirection);
    }

    const std::optional<Coordinate> playerColumn = m_player.column();
    if (!playerColumn.has_value())
    {
        return;
    }

    if (m_player.alive() &&
        m_playerRequestedDirection == Player::Direction::None && !m_player.isMoving())
    {
        m_player.stop();
    }

    const float playerHeight = m_player.getPosition().y;
    const float columnHeight =
        float(this->columnHeight(playerColumn.value())) * BOX_SIZE;
    if (playerHeight > columnHeight)
    {
        if (!m_player.isMoving())
        {
            m_player.move(Player::Direction::Down);
        }
        return;
    }

    if (m_player.direction() == Player::Direction::Down)
    {
        m_player.stopFalling();
        m_player.move(Player::Direction::None);
    }
}


void World::updateBoxes(const Duration &elapsed)
{
    std::map<BoxPtr, BoxIndexChange> updateIndex;

    for (auto &column : m_boxesStatic)
    {
        for (const Object::Id boxId : column)
        {
            if (boxId == NULL_ID)
            {
                continue;
            }
            BoxPtr &box = m_boxes[boxId];
            const BoxIndexChange change = updateBox(*box.get(), elapsed);
            if (change != BoxIndexChange::None)
            {
                updateIndex[box] = change;
            }
        }
    }
    for (auto it = m_boxesMoving.begin(); it != m_boxesMoving.end(); ++it)
    {
        const Object::Id boxId = *it;
        BoxPtr &box = m_boxes[boxId];
        const BoxIndexChange change = updateBox(*box.get(), elapsed);
        if (change != BoxIndexChange::None)
        {
            updateIndex[box] = change;
        }
    }

    // Обновление индексов.
    for (auto it = updateIndex.cbegin(); it != updateIndex.cend(); ++it)
    {
        updateBoxesIndex(it->first, it->second);
    }
}


World::BoxIndexChange World::updateBox(Box &box, const Duration &elapsed)
{
    box.update(elapsed);

    if (box.isBlowing())
    {
        return BoxIndexChange::None;
    }

    if (box.isBlowed())
    {
        // Если ящик взорвался в воздухе, то он был сбит игроком.
        return box.getPosition().y > 0
            ? BoxIndexChange::BlowedByPlayer
            : BoxIndexChange::None;
    }

    const std::optional<Coordinate> boxColumn = box.column();
    if (!boxColumn.has_value())
    {
        // Пока ящик не переместился в определённую колонку
        // ничего обновлять не требуется.
        return BoxIndexChange::None;
    }

    if (boxHitsPlayer(box))
    {
        box.blow();
        if (!(m_player.direction() & Player::Direction::Up))
        {
            stop();
        }
        return BoxIndexChange::None;
    }

    const Coordinate column = boxColumn.value();
    const float boxHeight = box.getPosition().y;
    const float columnHeight = this->columnHeight(column) * BOX_SIZE;
    if (boxHeight > columnHeight)
    {
        // Под ящиком есть пустота => ящик падает.
        if (!box.isFalling())
        {
            box.move(Box::Direction::Down);
            return BoxIndexChange::StartFalling;
        }
        return BoxIndexChange::None;
    }

    // Ящик стоит на стопке.
    // Не сброшенное направление - признак того,
    // что движение только завершилось.
    if (box.direction() != Box::Direction::None)
    {
        box.stopFalling();
        box.move(Box::Direction::None);
        return BoxIndexChange::Stopped;
    }

    return BoxIndexChange::None;
}


void World::updateBoxesIndex(const BoxPtr &box, BoxIndexChange change)
{
    switch (change)
    {
    case BoxIndexChange::StartFalling:
        {
            auto &boxesInColumn = m_boxesStatic[box->column().value()];
            auto it = std::find(
                boxesInColumn.begin(),
                boxesInColumn.end(),
                box->id());
            if (it != boxesInColumn.cend())
            {
                *it = NULL_ID;
            }
            m_boxesMoving.insert(box->id());
        }
        break;
    case BoxIndexChange::Stopped:
        {
            const std::optional<Coordinate> boxRow = box->row();
            if (boxRow.has_value())
            {
                m_boxesMoving.erase(box->id());
                m_boxesStatic[box->column().value()][boxRow.value()] = box->id();
            }
        }
        break;
    case BoxIndexChange::BlowedByPlayer:
        m_boxesMoving.erase(box->id());
        m_boxes.erase(box->id());
        break;
    default:
        break;
    }
}


void World::updateCrane(Crane &crane, const Duration &elapsed)
{
    crane.update(elapsed);

    if (crane.boxId() != NULL_ID)
    {
        BoxPtr &box = m_boxes[crane.boxId()];
        const std::optional<Coordinate> boxColumn = box->column();
        if (boxColumn.has_value() &&
            boxColumn.value() == crane.dropColumn() &&
            canDropBox(crane.boxId(), crane.dropColumn()))
        {
            dropBox(crane);
            crane.drop();
        }
        else
        {
            box->setPosition(sum(crane.getPosition(), BOX_OFFSET));
        }
    }

    if (crane.readyToReset())
    {
        // Кран проехал всё игровое поле.
        if (m_player.alive())
        {
            resetCrane(crane);
        }
        else
        {
            crane.stop();
        }
    }
}


void World::pushBox(
    Coordinate row,
    Coordinate column,
    Box::Direction direction)
{
    const Object::Id boxId = m_boxesStatic[column][row];
    BoxPtr &box = m_boxes[boxId];
    box->move(direction);
    m_boxesStatic[column][row] = NULL_ID;
    m_boxesMoving.insert(box->id());
}


bool World::boxHitsPlayer(const Box &box) const noexcept
{
    const sf::Vector2f &playerPosition = m_player.getPosition();
    const sf::Vector2f &boxPosition = box.getPosition();
    const float heightRange = boxPosition.y - playerPosition.y;
    const bool result =
        std::abs(playerPosition.x - boxPosition.x) < BOX_SIZE / 2 &&
        heightRange < Player::height() && heightRange > 0;
    return result;
}


bool World::canDropBox(Object::Id boxId, Coordinate column) const
{
    const Coordinates playerCoordinates = m_player.coordinates();
    if (playerCoordinates.column.has_value() &&
        playerCoordinates.column.value() == column &&
        playerCoordinates.row.has_value() &&
        playerCoordinates.row.value() == ROWS_WITH_ALLOWED_JUMP)
    {
        // Нельзя сбрасывать ящик на игрока, стоящего непосредственно под краном.
        return false;
    }

    if (columnHeight(column) >= BOXES_ROWS)
    {
        // Стопка ящиков достигла максимальной высоты.
        return false;
    }

    // Проверка наличия падающего ящика в этой же колонке,
    // чтобы избежать наложения.
    const float boxDropAltitude = CRANE_VERTICAL_POSITION + BOX_OFFSET.y;
    for (const auto &boxInfo : m_boxes)
    {
        const BoxPtr &box = boxInfo.second;
        if (box->column() == column &&
            box->id() != boxId &&
            std::abs(boxDropAltitude - box->getPosition().y) < BOX_SIZE)
        {
            return false;
        }
    }

    return true;
}


void World::dropBox(Crane &crane)
{
    m_boxesMoving.insert(crane.boxId());
    crane.drop();
    // Очки за сборс начисляются, если игрок жив.
    if (m_player.alive())
    {
        m_score += DROP_BOX_SCORE;
    }
}


bool World::bottomRowFilled() const
{
    for (const auto &column : m_boxesStatic)
    {
        if (column.front() == NULL_ID)
        {
            return false;
        }
    }
    return true;
}


bool World::blowBottomRow()
{
    bool addScore = false;
    bool rowBlowed = true;
    for (auto &column : m_boxesStatic)
    {
        const Object::Id bottomBoxId = column.front();
        BoxPtr &box = m_boxes[bottomBoxId];
        if (!box->isBlowed())
        {
            addScore |= box->blow();
            rowBlowed = false;
        }
        else
        {
            m_boxes.erase(bottomBoxId);
            column.front() = NULL_ID;
        }
    }
    if (addScore)
    {
        const std::size_t cranesQuantity = this->cranesQuantity();
        m_score += cranesQuantity * BLOW_BOTTOM_ROW_SCORE_MULTIPLIER;
    }
    return rowBlowed;
}


Coordinate World::columnHeight(Coordinate column) const noexcept
{
    std::size_t row = 0;
    for (; row < m_boxesStatic[column].size(); ++row)
    {
        if (m_boxesStatic[column][row] == NULL_ID)
        {
            break;
        }
    }
    return row;
}


void World::renderCrane(Crane &crane, sf::RenderTarget &target) const
{
    target.draw(crane, m_transform);
    if (crane.boxId() != NULL_ID)
    {
        const BoxPtr &box = m_boxes.at(crane.boxId());
        target.draw(*box, m_transform);
    }
}


void World::stop()
{
    LOG_INFO("Game over. Score: " << m_score << '.');
    m_player.setAlive(false);

    NumberPtr number = std::make_shared<Number>(m_score);
    number->setPosition(SCORE_POSITION_STOPPED);
    m_scoreFigures.push_back(number);
}
