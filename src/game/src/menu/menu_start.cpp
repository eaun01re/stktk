#include <game/menu/menu_start.h>


MenuStart::MenuStart()
{
    setup();
}


boost::signals2::connection MenuStart::connectStart(const Slot &slot)
{
    return connectLeft(slot);
}


boost::signals2::connection MenuStart::connectOptions(const Slot &slot)
{
    return connectRight(slot);
}


void MenuStart::setup()
{
    m_buttonLeft.setCaption(U"Начать");
    m_buttonRight.setCaption(U"Опции");
}
