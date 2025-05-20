#include <game/menu/menu_level.h>

#include <game/config.h>


namespace
{

const std::array<std::u32string, 3> MENU_ITEMS_CAPTIONS
{
    U"1 кран",
    U"2 крана",
    U"3 крана",
};

}


MenuLevel::MenuLevel(IMenuObserver *parent)
    : Menu(parent)
{
    setup();
}


void MenuLevel::setup()
{
    makeFrame();

    for (std::size_t i = 0; i < m_menuItems.size(); ++i)
    {
        m_menuItems[i] =
            std::make_shared<MenuItem>(MENU_ITEMS_CAPTIONS[i], true);
        m_menuItems[i]->setAction(
            false,
            { SELECT_CAPTION, [this, i](){ setCheckedItem(i); } });
    }

    setItems(
    {
        m_menuItems[0],
        m_menuItems[1],
        m_menuItems[2],
    });

    setCheckedItem(Config::instance().cranesQuantity - 1);
}


void MenuLevel::onClosing()
{
    const std::size_t index = selectedItem();
    if (index >= m_menuItems.size())
    {
        return;
    }

    Config &config = Config::instance();
    config.cranesQuantity = index + 1;
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


void MenuLevel::setCheckedItem(std::size_t index)
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
