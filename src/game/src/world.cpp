#include <game/world.h>

#include <optional>
#include <set>

#include <game/config.h>
#include <game/initial_position.h>
#include <game/log.h>
#include <game/objects/hourglass.h>
#include <game/objects/number.h>
#include <game/sound_system.h>

#include "math/math.h"


namespace
{

constexpr sf::Keyboard::Key KEY_PAUSE = sf::Keyboard::Key::Num0;
constexpr sf::Keyboard::Key KEY_PAUSE2 = sf::Keyboard::Key::Numpad0;

constexpr sf::Keyboard::Key KEY_RESTART = sf::Keyboard::Key::Space;

constexpr sf::Keyboard::Key KEY_ADD_CRANE = sf::Keyboard::Key::C;

constexpr sf::Keyboard::Key KEY_GOD_MODE = sf::Keyboard::Key::G;


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

/// Положение окна, отображающего текущий счёт игры
/// относительно левого верхнего угла.
const sf::Vector2f SCORE_POSITION_PAUSED(26, 34);
/// Положение окна, отображающего итоговый счёт игры
/// относительно левого верхнего угла.
const sf::Vector2f SCORE_POSITION_STOPPED(26, 24);
/// Положение песочных часов, отображаемых во время паузы
/// относительно левого верхнего угла.
const sf::Vector2f HOURGLASS_POSITION(44, 12);

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

/*!
 * Возвращает длину пути, которую кран должен преодолеть в рамках одного цикла.
 * NOTE: Длина должна быть такой, чтобы позволить крану плавно появляться
 * из-за одного края игровой области и плавноц заходить за другой край.
 * \param[in] craneWidth Ширина крана.
 * \return Длина одного цикла курсирования крана.
 */
unsigned int craneCycleLength(const unsigned int craneWidth)
{
    return craneWidth + MAX_CRANES_QUANTITY * CRANE_INTERVAL;
}

Player::Direction directionByKey(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::Key::Left:
    case sf::Keyboard::Key::A:
    case sf::Keyboard::Key::Numpad4:
        return Player::Direction::Left;
    case sf::Keyboard::Key::Right:
    case sf::Keyboard::Key::D:
    case sf::Keyboard::Key::Numpad6:
        return Player::Direction::Right;
    case sf::Keyboard::Key::Up:
    case sf::Keyboard::Key::W:
    case sf::Keyboard::Key::Numpad8:
        return Player::Direction::Up;
    case sf::Keyboard::Key::Q:
    case sf::Keyboard::Key::Numpad7:
        return Player::Direction::UpLeft;
    case sf::Keyboard::Key::E:
    case sf::Keyboard::Key::Numpad9:
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
    : m_player(
        std::bind(&World::onPlayerMoveFinished, this, std::placeholders::_1))
{
    setup();
}


void World::start(const std::optional<unsigned int> &positionIndex)
{
    // Удаление старых объектов.
    clearObjects();
    m_scoreFigures.clear();

    // Инициализация ящиков.
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

    // Инициализация игрока.
    setPlayerColumn(playerColumn);
    m_player.setAlive(true);

    addCranes(Config::instance().cranesQuantity);

    m_score = 0;
    m_paused = false;
    resume();
}


void World::setDebugMode(bool value)
{
    m_debugMode = value;
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
    Object::Id pushedBoxId = NULL_ID;
    if (!canPlayerMove(
            playerCoordinates.row,
            playerCoordinates.column,
            direction,
            pushedBoxId))
    {
        return;
    }

    const bool push = pushedBoxId != NULL_ID;
    if (push)
    {
        pushBox(
            *m_boxes[pushedBoxId].get(),
            direction == Player::Direction::Left
                ? Box::Direction::Left
                : Box::Direction::Right);
    }
    m_player.move(direction, push);

    if (direction & Player::Direction::Up)
    {
        playSound(ResourceLoader::SoundId::Jump);
    }
    else if (push)
    {
        playSound(ResourceLoader::SoundId::Push);
    }
}


void World::draw(sf::RenderTarget& target, sf::RenderStates) const
{
    target.draw(m_background);

    // Отрисковка в преобразованной системе координат.
    {
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
    }

    // Отображение счёта.
    for (const auto &figure : m_scoreFigures)
    {
        target.draw(*figure);
    }

    // Отображение переднего плана.
    target.draw(m_foreground);
}


bool World::handleKeyPressed(const sf::Keyboard::Key key)
{
    if (!m_player.alive())
    {
        if (key == KEY_RESTART)
        {
            start();
            return true;
        }
        return false;
    }

    switch (key)
    {
    case KEY_ADD_CRANE:
        if (m_debugMode)
        {
            addCrane();
        }
        break;
    case KEY_GOD_MODE:
        m_godMode = !m_godMode;
        break;
    case KEY_PAUSE:
    case KEY_PAUSE2:
        togglePause();
        return true;
    default:
        break;
    }

    const Player::Direction requestedDirection = directionByKey(key);
    if (requestedDirection != Player::Direction::None)
    {
        requestMovePlayer(requestedDirection);
        return true;
    }

    return false;
}


void World::handleKeyReleased(const sf::Keyboard::Key key)
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

    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_background.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Background));
    m_foreground.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Foreground));
    m_foreground.setColor(BACKGROUND_COLOR);
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
        if (columnHeight(i) == MAX_BOXES_IN_COLUMN)
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


BoxPtr World::makeBox()
{
    BoxPtr box = std::make_shared<Box>(
        std::bind(&World::onBoxMoveStarted, this, std::placeholders::_1),
        std::bind(&World::onBoxMoveFinished, this, std::placeholders::_1));
    m_boxes.emplace(box->id(), box);
    return box;
}


BoxPtr World::addBox(Coordinate row, Coordinate column)
{
    BoxPtr box = makeBox();
    box->setPosition(sf::Vector2f(column * BOX_SIZE, row * BOX_SIZE));
    m_boxesStatic[column][row] = box->id();
    return box;
}


void World::addCranes(std::uint8_t cranesQuantity)
{
    // Ограничение стартового количества кранов.
    cranesQuantity = std::clamp(
        cranesQuantity,
        std::uint8_t(1),
        MAX_INITIAL_CRANES_QUANTITY);
    for (std::uint8_t i = 0; i < cranesQuantity; ++i)
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

    LOG_DEBUG("Adding new crane.");
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
        for (std::size_t i = 1; i < m_cranes.size(); ++i)
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
                // Ячейча не видна.
                break;
            }
        }
        if (offset > 0)
        {
            offset -= craneCycleLength(crane->width());
            LOG_DEBUG("Correcting crane offset to " << offset << '.');
        }
        craneOffset = offset;
    }

    LOG_DEBUG("Added crane №" << craneIndex << '.');
    resetCrane(*crane.get(), std::abs(craneOffset));
    m_cranes[craneIndex] = crane;
}


std::size_t World::cranesQuantity() const noexcept
{
    std::size_t cranesQuantity = 0;
    for (std::size_t i = 0; i < m_cranes.size(); ++i)
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
        offsetLength + craneCycleLength(crane.width());
    crane.reset(position, left, movementLength);
    std::uniform_int_distribution<std::mt19937::result_type> distributionColumn(
        0,
        m_boxesStatic.size() - 1);
    crane.setDropColumn(distributionColumn(m_randomEngine));

    if (!crane.isLoaded())
    {
        const BoxPtr box = makeBox();
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
    const Player::Direction direction,
    Object::Id &pushedBoxId) const
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
        return canPlayerMoveHorizontal(row, column, true, pushedBoxId);
    case Player::Direction::Right:
        return canPlayerMoveHorizontal(row, column, false, pushedBoxId);
    case Player::Direction::UpLeft:
        return canPlayerMoveDiagonal(row, column, true);
    case Player::Direction::UpRight:
        return canPlayerMoveDiagonal(row, column, false);
    case Player::Direction::Up:
        return canPlayerMoveVertical(row, column);
    default:
        return false;
    }
}


bool World::canPlayerMoveHorizontal(
    Coordinate row,
    Coordinate column,
    bool left,
    Object::Id &pushedBoxId) const
{
    pushedBoxId = NULL_ID;

    // Проверка выхода за границы области перемещения.
    const bool canMove =
        (left && column >= 1) || (!left && column <= BOXES_COLUMNS - 2);
    if (!canMove)
    {
        return false;
    }

    // Проверка столкновения с падающими ящиками.
    const int k = left ? -1 : 1;
    for (const Object::Id boxId : m_boxesMoving)
    {
        const BoxPtr &box = m_boxes.at(boxId);

        const std::optional<Coordinate> boxColumn = box->column();
        if (!boxColumn.has_value() ||
            (int(boxColumn.value()) != int(column) + k))
        {
            // Ящик не находится в соседней колонке.
            continue;
        }

        // Ограничение ящиками в соседней колонке.
        if ((box->getPosition().y >= m_player.getPosition().y + BOX_SIZE) &&
            (box->getPosition().y <= m_player.getPosition().y + Player::height()))
        {
            // Ящик блокирует перемещение, т.к. находится на высоте не ниже
            // уровня пояса игрока и не выше его головы.
            return false;
        }

        if ((box->getPosition().y < m_player.getPosition().y + BOX_SIZE) &&
            (box->getPosition().y > m_player.getPosition().y))
        {
            // Ящик находится на допустимой для толкания высоте.

            if (canPushBox(*box.get(), boxColumn.value(), left))
            {
                // Ящик можно толкать.
                pushedBoxId = boxId;
            }
            else
            {
                // Толкать ящик мешают другие ящики.
                return false;
            }
        }
    }
    if (pushedBoxId != NULL_ID)
    {
        // Если игрок начинает толкать падающий ящик, то в начале толкания
        // он мгновенно помещается на ту же высоту где находится этот ящик.
        // TODO: Выяснить, что происходит, если после этого подъема игрок
        // задевает другой ящик.
        const Coordinate playerColumn = m_player.column().value();
        const float pushedBoxHeight = m_boxes.at(pushedBoxId)->getPosition().y;
        for (const Object::Id boxId : m_boxesMoving)
        {
            const BoxPtr &box = m_boxes.at(boxId);
            const std::optional<Coordinate> boxColumn = box->column();
            if (boxColumn.has_value()
                && boxColumn.value() == playerColumn
                && pushedBoxHeight + Player::height() > box->getPosition().y
                && m_player.getPosition().y + Player::height() < box->getPosition().y)
            {
                return false;
            }
        }
        return true;
    }

    // В соседней колонке нет падающих ящиков,
    // с которыми возможно взаимодействие.

    // Переход на соседнюю стопку без толкания ящика.
    // Игрок должен стоять на ящике, либо встать на ящик после перемещения.
    const Coordinate nextColumn = column + (left ? -1 : 1);
    const Coordinate nextColumnHeight = columnHeight(nextColumn);
    const Coordinate currentColumnHeight = columnHeight(column);
    if (row == nextColumnHeight ||
        (row == currentColumnHeight && row > nextColumnHeight))
    {
        return true;
    }

    // Переход на соседнюю стопку с толканием ящика.
    // Высота соседней стопки должна быть не более чем на 1 больше, чем у текущей.
    if (row != nextColumnHeight - 1)
    {
        return false;
    }
    // Высота следующей после соседней стопки должна быть не более,
    // чем у текущей.
    const BoxPtr &topBox =
        m_boxes.at(m_boxesStatic.at(nextColumn).at(nextColumnHeight - 1));
    const bool canPush = canPushBox(*topBox.get(), nextColumn, left);
    if (canPush)
    {
        pushedBoxId = topBox->id();
    }
    return canPush;
}


bool World::canPlayerMoveVertical(Coordinate row, Coordinate column) const
{
    return row < ROWS_WITH_ALLOWED_JUMP && row == columnHeight(column);
}


bool World::canPlayerMoveDiagonal(
    Coordinate row,
    Coordinate column,
    bool left) const
{
    // Проверка выхода за границы области прыжка.
    bool allowed =
        row < ROWS_WITH_ALLOWED_JUMP &&
        ((left && (column > 1)) || (!left && (column < BOXES_COLUMNS - 2)));
    if (!allowed)
    {
        return false;
    }

    // Проверка столкновения с неподвижными ящиками.
    const int k = left ? -1 : 1;
    allowed =
        row == columnHeight(column) &&
        row >= columnHeight(column + k) &&
        row + 1 >= columnHeight(column + 2 * k);
    if (!allowed)
    {
        return false;
    }

    // Проверка столкновения с падающими ящиками.
    // Т.к. вертикальные скорости игрока и ящика известны и равны
    // можно оценить их взаимное положение через определённое время.
    for (const Object::Id boxId : m_boxesMoving)
    {
        const BoxPtr &box = m_boxes.at(boxId);

        const std::optional<Coordinate> boxColumn = box->column();
        if (!boxColumn.has_value())
        {
            continue;
        }

        // Ограничение ящиками в соседней колонке.
        // В момент нахождения в наивысшей точке прыжка
        // игрок не должен касаться ящика головой.
        if (std::abs(int(boxColumn.value()) - int(column)) == 1 &&
            box->getPosition().y - BOX_SIZE
            < m_player.getPosition().y + Player::height() + BOX_SIZE)
        {
            return false;
        }

        // Ограничение ящиками в колонке где завершится прыжок.
        // Ящик не ограничивает прыжок, если он еще падает, но либо успеет
        // приземлиться к моменту когда игрок будет на него становиться, либо
        // будет ещё достаточно высоко и не коснётся игрока в конце прыжка.
        if (std::abs(int(boxColumn.value()) - int(column)) == 2 &&
            (box->getPosition().y >= m_player.getPosition().y + BOX_SIZE) &&
            (box->getPosition().y - 2 * BOX_SIZE
             < m_player.getPosition().y + Player::height() + BOX_SIZE))
        {
            return false;
        }
    }
    return true;
}


bool World::canPushBox(Box &box, Coordinate boxColumn, bool left) const
{
    // Проверка наличия места для толкания.
    const bool enoughSpace =
        (left && boxColumn >= 1) || (!left && boxColumn <= BOXES_COLUMNS - 2);
    if (!enoughSpace)
    {
        return false;
    }

    // Проверка возможных пересечений с покоящимися ящиками.
    const int k = left ? -1 : 1;
    if (box.getPosition().y < columnHeight(boxColumn + k) * BOX_SIZE)
    {
        return false;
    }

    // Проверка возможных пересечений с ящиками, падающими в соседней колонке.
    for (const Object::Id boxId : m_boxesMoving)
    {
        const BoxPtr &boxToCheck = m_boxes.at(boxId);

        const std::optional<Coordinate> boxToCheckColumn = boxToCheck->column();
        if (!boxToCheckColumn.has_value() ||
            std::abs(int(boxToCheckColumn.value()) - int(boxColumn)) != 1)
        {
            // Ящик не находится в соседней колонке.
            continue;
        }
        if (std::abs(box.getPosition().y - boxToCheck->getPosition().y) < BOX_SIZE)
        {
            // Ящик упирается в другой ящик из соседней колонки.
            return false;
        }
    }
    return true;
}


Player::Direction World::playerNextDirection(
    Player::Direction direction,
    Coordinate row,
    Coordinate column) const
{
    Player::Direction result = Player::Direction::None;
    if (direction == Player::Direction::UpLeft)
    {
        result = row == columnHeight(column - 1)
            ? Player::Direction::Left
            : Player::Direction::DownLeft;
    }
    else if (direction == Player::Direction::UpRight)
    {
        result = row == columnHeight(column + 1)
            ? Player::Direction::Right
            : Player::Direction::DownRight;
    }
    return result;
}


void World::onPlayerMoveFinished(Player::Direction lastDirection)
{
    const Player::Direction nextDirection = playerNextDirection(
        lastDirection,
        m_player.row().value(),
        m_player.column().value());
    m_player.move(nextDirection, false);
}


void World::onBoxMoveStarted(Object::Id boxId)
{
    BoxPtr &box = m_boxes[boxId];
    const std::optional<Coordinate> boxColumn = box->column();
    const std::optional<Coordinate> boxRow = box->row();
    if (!boxColumn.has_value() || !boxRow.has_value())
    {
        return;
    }

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


void World::onBoxMoveFinished(Object::Id boxId)
{
    BoxPtr &box = m_boxes[boxId];
    const std::optional<Coordinate> boxColumn = box->column();
    const std::optional<Coordinate> boxRow = box->row();
    if (!boxColumn.has_value() || !boxRow.has_value())
    {
        return;
    }

    m_boxesMoving.erase(boxId);
    m_boxesStatic[boxColumn.value()][boxRow.value()] = boxId;
}


void World::updatePlayer(const Duration &elapsed)
{
    m_player.update(elapsed);

    if (m_playerRequestedDirection != Player::Direction::None
        && (m_player.direction() == Player::Direction::None || m_player.isFalling()))
    {
        // Начало движения игрока в запрошенном направлении, если это возможно.
        movePlayer(m_playerRequestedDirection);
        m_playerRequestedDirection = Player::Direction::None;
    }

    const std::optional<Coordinate> playerColumn = m_player.column();
    if (!playerColumn.has_value())
    {
        return;
    }

    if (m_playerRequestedDirection == Player::Direction::None
        && !m_player.isMoving())
    {
        m_player.idle();
    }

    const float columnHeight =
        float(this->columnHeight(playerColumn.value())) * BOX_SIZE;
    if (m_player.getPosition().y > columnHeight)
    {
        if (!m_player.isMoving())
        {
            m_player.move(Player::Direction::Down);
        }
        return;
    }

    // Игрок завершил падение.
    if (m_player.direction() == Player::Direction::Down)
    {
        m_player.stopFalling();
        m_player.move(Player::Direction::None);
        playSound(ResourceLoader::SoundId::Land);
    }
}


void World::updateBoxes(const Duration &elapsed)
{
    // Обновление стоящих ящиков.
    for (auto &column : m_boxesStatic)
    {
        for (const Object::Id boxId : column)
        {
            if (boxId == NULL_ID)
            {
                continue;
            }
            BoxPtr &box = m_boxes[boxId];
            updateBox(*box.get(), elapsed);
        }
    }

    // Обновление движущихся ящиков.
    // Копирование списка ящиков, т.к. он может меняться внутри цикла.
    auto boxesMoving = m_boxesMoving;
    for (auto it = boxesMoving.begin(); it != boxesMoving.end(); ++it)
    {
        const Object::Id boxId = *it;
        BoxPtr &box = m_boxes[boxId];
        const bool blowedByPlayer = updateBox(*box.get(), elapsed);
        if (blowedByPlayer)
        {
            m_boxesMoving.erase(box->id());
            m_boxes.erase(box->id());
        }
    }
}


bool World::updateBox(Box &box, const Duration &elapsed)
{
    box.update(elapsed);

    if (box.isBlowing())
    {
        return false;
    }

    if (box.isBlowed())
    {
        // Если ящик взорвался в воздухе, то он был сбит игроком.
        return box.getPosition().y > 0;
    }

    if (boxHitsPlayer(box))
    {
        box.blow();
        if (m_player.alive())
        {
            // NOTE: Можно сбивать ящик прыжком по диагонали.
            if (!m_godMode && !(m_player.direction() & Player::Direction::Up))
            {
                // Ящик упал на игрока, не находящегося в прыжке => конец игры.
                stop();
            }
            else
            {
                playSound(ResourceLoader::SoundId::Blow);
            }
        }
        return false;
    }

    // Проверка необходимости начала/остановки падения.
    const std::optional<Coordinate> boxColumn = box.column();
    float columnHeight = 0;
    if (!boxColumn.has_value())
    {
        // Ящик находится между колонками.
        const float column = box.getPosition().x / BOX_SIZE;
        Coordinate column1 = std::floor(column);
        Coordinate column2 = std::ceil(column);
        columnHeight = std::max(
            this->columnHeight(column1),
            this->columnHeight(column2)) * BOX_SIZE;
    }
    else
    {
        columnHeight = this->columnHeight(boxColumn.value()) * BOX_SIZE;
    }
    if (box.getPosition().y <= columnHeight)
    {
        if (box.isFalling())
        {
            box.stopFalling();
        }
    }
    else
    {
        if (!box.isFalling())
        {
            box.move(Box::Direction::Down);
            return false;
        }
    }

    return false;
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


void World::pushBox(Box &box, Box::Direction direction)
{
    const std::optional<Coordinate> column = box.column();
    const std::optional<Coordinate> row = box.row();
    if (column.has_value() &&
        row.has_value() &&
        m_boxesStatic.at(column.value()).at(row.value()) == box.id())
    {
        m_boxesStatic[column.value()][row.value()] = NULL_ID;
    }
    box.move(direction);
    m_boxesMoving.insert(box.id());
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
        playSound(ResourceLoader::SoundId::Score);
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


void World::playSound(ResourceLoader::SoundId id)
{
    SoundSystem::instance().playSound(id);
}


void World::stop()
{
    LOG_INFO("Game over. Score: " << m_score << '.');
    m_player.setAlive(false);
    playSound(ResourceLoader::SoundId::GameOver);

    NumberPtr number = std::make_shared<Number>(m_score);
    number->setPosition(SCORE_POSITION_STOPPED);
    m_scoreFigures.push_back(number);
}
