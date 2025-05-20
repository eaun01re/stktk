#include "game/log.h"
#include <game/menu/menu.h>

#include <game/consts.h>


namespace
{

constexpr sf::Keyboard::Key KEY_LEFT = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key KEY_RIGHT = sf::Keyboard::Key::Right;
constexpr sf::Keyboard::Key KEY_UP = sf::Keyboard::Key::Up;
constexpr sf::Keyboard::Key KEY_DOWN = sf::Keyboard::Key::Down;
constexpr sf::Keyboard::Key KEY_BACK = sf::Keyboard::Key::Escape;
const sf::Vector2u FRAME_SIZE(
    SCREEN_SIZE.x - 6,
    SCREEN_SIZE.y - BUTTON_SIZE.y - 4);
const sf::Vector2u FRAME_POSITION(
    (SCREEN_SIZE.x - FRAME_SIZE.x) / 2,
    (SCREEN_SIZE.y - BUTTON_SIZE.y - FRAME_SIZE.y) / 2);
const sf::Vector2u FRAME_ITEMS_OFFSET(1, 4);
const sf::Vector2f SELECTION_SIZE(FRAME_SIZE.x - 3, BUTTON_SIZE.y);
constexpr std::size_t MAX_ITEMS_VISIBLE = 3;

/*!
 * Возвращает список графических примитивов, составляющих рамку меню.
 * \return Прямоугольники, составляющие рамку.
 */
std::list<sf::RectangleShape> makeFrame()
{
    sf::RectangleShape background(sf::Vector2f(
        FRAME_SIZE.x - 3,
        FRAME_SIZE.y - 3));
    background.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 1,
        FRAME_POSITION.y + 1));
    background.setFillColor(BACKGROUND_COLOR);

    sf::RectangleShape topLine(sf::Vector2f(FRAME_SIZE.x - 5, 1));
    topLine.setPosition(sf::Vector2f(FRAME_POSITION.x + 2, FRAME_POSITION.y));
    topLine.setFillColor(TEXT_COLOR);

    sf::RectangleShape rightLine(sf::Vector2f(1, FRAME_SIZE.y - 4));
    rightLine.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 2,
        FRAME_POSITION.y + 2));
    rightLine.setFillColor(TEXT_COLOR);
    sf::RectangleShape rightLine2(sf::Vector2f(1, FRAME_SIZE.y - 6));
    rightLine2.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 1,
        FRAME_POSITION.y + 3));
    rightLine2.setFillColor(TEXT_COLOR);

    sf::RectangleShape bottomLine(sf::Vector2f(FRAME_SIZE.x - 4, 1));
    bottomLine.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 2,
        FRAME_POSITION.y + FRAME_SIZE.y - 2));
    bottomLine.setFillColor(TEXT_COLOR);
    sf::RectangleShape bottomLine2(sf::Vector2f(FRAME_SIZE.x - 6, 1));
    bottomLine2.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 3,
        FRAME_POSITION.y + FRAME_SIZE.y - 1));
    bottomLine2.setFillColor(TEXT_COLOR);

    sf::RectangleShape leftLine(sf::Vector2f(1, FRAME_SIZE.y - 5));
    leftLine.setPosition(sf::Vector2f(FRAME_POSITION.x, FRAME_POSITION.y + 2));
    leftLine.setFillColor(TEXT_COLOR);

    sf::RectangleShape topLeftDot(sf::Vector2f(1, 1));
    topLeftDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 1,
        FRAME_POSITION.y + 1));
    topLeftDot.setFillColor(TEXT_COLOR);
    sf::RectangleShape topRightDot(sf::Vector2f(1, 1));
    topRightDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 3,
        FRAME_POSITION.y + 1));
    topRightDot.setFillColor(TEXT_COLOR);
    sf::RectangleShape bottomRightDot(sf::Vector2f(1, 1));
    bottomRightDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + FRAME_SIZE.x - 3,
        FRAME_POSITION.y + FRAME_SIZE.y - 3));
    bottomRightDot.setFillColor(TEXT_COLOR);
    sf::RectangleShape bottomLeftDot(sf::Vector2f(1, 1));
    bottomLeftDot.setPosition(sf::Vector2f(
        FRAME_POSITION.x + 1,
        FRAME_POSITION.y + FRAME_SIZE.y - 3));
    bottomLeftDot.setFillColor(TEXT_COLOR);

    const std::list<sf::RectangleShape> frame =
    {
        background,
        topLine,
        rightLine,
        rightLine2,
        bottomLine,
        bottomLine2,
        leftLine,
        topLeftDot,
        topRightDot,
        bottomRightDot,
        bottomLeftDot,
    };
    return frame;
}

}


Menu::Menu(IMenuObserver *parent)
    : m_parent(parent)
{
    const unsigned int buttonTop = SCREEN_SIZE.y - BUTTON_SIZE.y;
    m_buttonLeft.setPosition(
        sf::Vector2f(0, buttonTop));
    m_buttonRight.setPosition(
        sf::Vector2f(SCREEN_SIZE.x - BUTTON_SIZE.x, buttonTop));
}


bool Menu::handleKeyPressed(const sf::Keyboard::Key key)
{
    if (m_submenu != nullptr)
    {
        return m_submenu->handleKeyPressed(key);
    }

    switch (key)
    {
    case KEY_LEFT:
        onLeftActivated();
        return true;
    case KEY_RIGHT:
        onRightActivated();
        return true;
    case KEY_UP:
        moveSelection(false);
        return true;
    case KEY_DOWN:
        moveSelection(true);
        return true;
    case KEY_BACK:
        close(false);
        return true;
    default:
        break;
    }

    return false;
}


void Menu::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_submenu != nullptr)
    {
        m_submenu->draw(target, states);
        return;
    }

    target.draw(m_buttonLeft);
    target.draw(m_buttonRight);

    for (const sf::RectangleShape &rectangle : m_frame)
    {
        target.draw(rectangle);
    }

    if (m_items.empty())
    {
        return;
    }

    target.draw(*m_selection);
    for (std::size_t i = 0; i < m_items.size() && i < MAX_ITEMS_VISIBLE; ++i)
    {
        const std::size_t itemIndex = visibleItemIndex(i);
        target.draw(*m_items[itemIndex]);
    }
}


void Menu::setSubmenu(std::unique_ptr<Menu> submenu)
{
    m_submenu = std::move(submenu);
}


void Menu::makeFrame()
{
    m_frame = ::makeFrame();
}


void Menu::moveSelection(bool down)
{
    if (m_items.empty())
    {
        return;
    }

    const int inc = down ? 1 : -1;
    m_currentItemIndex =
        (m_currentItemIndex + m_items.size() + inc) % m_items.size();

    if (m_items.size() <= MAX_ITEMS_VISIBLE)
    {
        m_selectionPosition = m_currentItemIndex;
    }
    else if (down)
    {
        m_selectionPosition = 1;
    }
    updateSelection();

    updateItems();
}


void Menu::onLeftActivated()
{
    m_buttonLeft.pressed();
}


void Menu::onRightActivated()
{
    m_buttonRight.pressed();
}


void Menu::onClosing()
{
}


void Menu::close(bool result)
{
    LOG_DEBUG("Closing menu " << this << ".");
    onClosing();

    if (m_parent != nullptr)
    {
        m_parent->childClosing(result);
    }
}


void Menu::setItems(const std::vector<std::shared_ptr<MenuItem>> &items)
{
    m_items = items;

    if (m_items.empty())
    {
        m_selection.reset();
        return;
    }

    m_selection.reset(new sf::RectangleShape(SELECTION_SIZE));
    m_selection->setPosition(sf::Vector2f(FRAME_POSITION + FRAME_ITEMS_OFFSET));
    m_selection->setFillColor(TEXT_COLOR);

    updateItems();
}


void Menu::childClosing(bool)
{
    LOG_DEBUG("Submenu closed.");
    m_submenu.reset();
}


void Menu::updateItems()
{
    const unsigned int y = FRAME_POSITION.y + FRAME_ITEMS_OFFSET.y + 1;
    for (std::size_t i = 0; i < m_items.size() && i < MAX_ITEMS_VISIBLE; ++i)
    {
        const std::size_t itemIndex = visibleItemIndex(i);
        m_items[itemIndex]->setPosition(sf::Vector2f(
            FRAME_POSITION.x + 2,
            y + i * SELECTION_SIZE.y));
        m_items[itemIndex]->setSelected(i == m_selectionPosition);
    }

    m_buttonLeft.setCaption(m_items[m_currentItemIndex]->action(true).caption);
    m_buttonLeft.setAction(m_items[m_currentItemIndex]->action(true).action);

    m_buttonRight.setCaption(m_items[m_currentItemIndex]->action(false).caption);
    m_buttonRight.setAction(m_items[m_currentItemIndex]->action(false).action);
}


void Menu::updateSelection()
{
    sf::Vector2f selectionPosition(FRAME_POSITION + FRAME_ITEMS_OFFSET);
    selectionPosition.y += SELECTION_SIZE.y * m_selectionPosition;
    m_selection->setPosition(selectionPosition);
}


void Menu::clearSubmenu()
{
    m_submenu.reset();
}


std::size_t Menu::visibleItemIndex(std::size_t visibleIndex) const noexcept
{
    if (m_items.size() <= MAX_ITEMS_VISIBLE)
    {
        return visibleIndex;
    }

    const int displayingItemsIndexOffset = m_selectionPosition;
    std::size_t index =
        m_currentItemIndex
        + visibleIndex
        - displayingItemsIndexOffset
        + m_items.size(); // Общее количество элементов прибавляется,
        // чтобы результат был положительным и работало деление по модулю.
    index %= m_items.size();
    return index;
}
