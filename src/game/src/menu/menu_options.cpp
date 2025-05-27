#include <game/menu/menu_options.h>


MenuOptions::MenuOptions()
{
    setup();
}


void MenuOptions::setup()
{
    makeFrame();

    m_menuItemStart = std::make_shared<MenuItem>(U"Начать");
    m_menuItemStart->action(false).caption = SELECT_CAPTION;

    m_menuItemSelectLevel = std::make_shared<MenuItem>(U"Уровень");
    m_menuItemSelectLevel->action(false).caption = SELECT_CAPTION;

    m_menuHighScore = std::make_shared<MenuItem>(U"Лучшие результ.");
    m_menuHighScore->action(false).caption = SELECT_CAPTION;

    m_menuItemSound = std::make_shared<MenuItem>(
        U"Звуки",
        MenuItem::Type::CheckBox);
    m_menuItemSound->action(false).caption = U"Измен.";
    m_menuItemSound->action(false).signal.connect(
        [this](){ m_menuItemSound->setChecked(!m_menuItemSound->checked()); });

    setItems(
    {
        m_menuItemStart,
        m_menuItemSelectLevel,
        m_menuHighScore,
        m_menuItemSound
    });
}


boost::signals2::connection MenuOptions::connectStart(const Slot &slot)
{
    return m_menuItemStart->action(false).signal.connect(slot);
}


boost::signals2::connection MenuOptions::connectSelectLevel(const Slot &slot)
{
    return m_menuItemSelectLevel->action(false).signal.connect(slot);
}


boost::signals2::connection MenuOptions::connectViewHighScore(const Slot &slot)
{
    return m_menuHighScore->action(false).signal.connect(slot);
}


bool MenuOptions::soundEnabled() const noexcept
{
    return m_menuItemSound->checked();
}


void MenuOptions::setSoundEnabled(bool value)
{
    m_menuItemSound->setChecked(value);
}
