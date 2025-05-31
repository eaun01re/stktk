#include <game/graphics/menu/menu_level.h>


namespace
{

const std::array<std::u32string, 3> MENU_ITEMS_CAPTIONS
{
    U"1 кран",
    U"2 крана",
    U"3 крана",
};

}


MenuLevel::MenuLevel()
{
    setup();
}


void MenuLevel::setup()
{
    makeFrame();

    for (std::size_t i = 0; i < m_menuItems.size(); ++i)
    {
        m_menuItems[i] = std::make_shared<MenuItem>(
            MENU_ITEMS_CAPTIONS[i],
            MenuItem::Type::RadioButton);
        m_menuItems[i]->action(false).caption = SELECT_CAPTION;
        m_menuItems[i]->action(false).signal.connect(
            [this, i](){ setSelectedItem(i); });
    }

    setItems(
    {
        m_menuItems[0],
        m_menuItems[1],
        m_menuItems[2],
    });
}


std::size_t MenuLevel::selectedItem() const noexcept
{
    std::size_t i = 0;
    for (; i < m_menuItems.size(); ++i)
    {
        if (m_menuItems[i]->checked())
        {
            return i;
        }
    }
    return i;
}


void MenuLevel::setSelectedItem(std::size_t index)
{
    if (index >= m_menuItems.size())
    {
        return;
    }

    for (auto &menuItem : m_menuItems)
    {
        menuItem->setChecked(false);
    }

    m_menuItems[index]->setChecked(true);
}
