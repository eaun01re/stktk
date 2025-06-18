#include <game/graphics/menu/menu_list.h>

#include <boost/signals2.hpp>

#include <game/consts.h>
#include <game/log.h>


namespace
{

constexpr sf::Keyboard::Key KEY_UP = sf::Keyboard::Key::Up;
constexpr sf::Keyboard::Key KEY_DOWN = sf::Keyboard::Key::Down;
const sf::Vector2u FRAME_ITEMS_OFFSET(1, 4);
const sf::Vector2f SELECTION_SIZE(FRAME_SIZE.x - 3, BUTTON_SIZE.y);
constexpr std::size_t MAX_ITEMS_VISIBLE = 3;

}


bool MenuList::handleKeyPressed(const sf::Keyboard::Key key)
{
    if (Menu::handleKeyPressed(key))
    {
        return true;
    }

    switch (key)
    {
    case KEY_UP:
        moveSelection(false);
        return true;
    case KEY_DOWN:
        moveSelection(true);
        return true;
    default:
        break;
    }

    return false;
}


void MenuList::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    Menu::draw(target, states);

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


void MenuList::moveSelection(bool down)
{
    const int inc = down ? 1 : -1;
    LOG_DEBUG("Move selection " << (down ? "down" : "up") << " requested.");

    if (m_items.empty())
    {
        return;
    }

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


void MenuList::setItems(const std::vector<std::shared_ptr<MenuItem>> &items)
{
    m_items = items;

    if (m_items.empty())
    {
        m_selection.reset();
        return;
    }

    // Добавление пустого пункта меню для обозначения конца меню.
    if (m_items.size() > 3)
    {
        std::shared_ptr<MenuItem> emptyItem = std::make_shared<MenuItem>(
            U"",
            MenuItem::Type::Empty);
        m_items.push_back(emptyItem);
    }

    m_selection.reset(new sf::RectangleShape(SELECTION_SIZE));
    m_selection->setPosition(sf::Vector2f(FRAME_POSITION + FRAME_ITEMS_OFFSET));
    m_selection->setFillColor(TEXT_COLOR);

    updateItems();
}


void MenuList::updateItems()
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
    connectLeft(m_items[m_currentItemIndex]->action(true).signal);

    m_buttonRight.setCaption(m_items[m_currentItemIndex]->action(false).caption);
    connectRight(m_items[m_currentItemIndex]->action(false).signal);
}


void MenuList::updateSelection()
{
    sf::Vector2f selectionPosition(FRAME_POSITION + FRAME_ITEMS_OFFSET);
    selectionPosition.y += SELECTION_SIZE.y * m_selectionPosition;
    m_selection->setPosition(selectionPosition);
}


std::size_t MenuList::visibleItemIndex(std::size_t visibleIndex) const noexcept
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
