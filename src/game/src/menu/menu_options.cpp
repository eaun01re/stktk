#include <game/menu/menu_options.h>

#include <game/config.h>
#include <game/log.h>
#include <game/menu/menu_level.h>
#include <game/sound_system.h>


MenuOptions::MenuOptions(IMenuObserver *parent)
    : Menu(parent)
{
    setup();
}


void MenuOptions::setup()
{
    makeFrame();

    std::shared_ptr<MenuItem> menuStart =
        std::make_shared<MenuItem>(U"Начать");
    menuStart->setAction(
        false,
        { SELECT_CAPTION, [this](){ close(true); } });

    std::shared_ptr<MenuItem> menuLevel =
        std::make_shared<MenuItem>(U"Уровень");
    menuLevel->setAction(
        false,
        { SELECT_CAPTION, std::bind(&MenuOptions::openSubmenu, this) });

    std::shared_ptr<MenuItem> menuScores =
        std::make_shared<MenuItem>(U"Лучшие результ.");
    menuScores->setAction(
        false,
        { SELECT_CAPTION, [](){ LOG_DEBUG("TODO: Not implemented"); } });

    m_menuSound = std::make_shared<MenuItem>(
        U"Звуки",
        MenuItem::Type::CheckBox);
    m_menuSound->setChecked(Config::instance().sound);
    m_menuSound->setAction(
        false,
        {
            U"Измен.",
            [this](){ m_menuSound->setChecked(!m_menuSound->checked()); }
        });

    setItems(
    {
        menuStart,
        menuLevel,
        menuScores,
        m_menuSound,
    });
}


void MenuOptions::openSubmenu()
{
    m_submenu.reset(new MenuLevel(this));
}


void MenuOptions::onClosing()
{
    Config::instance().sound = m_menuSound->checked();
    SoundSystem::instance().setEnabled(m_menuSound->checked());
}


void MenuOptions::childClosing(bool result)
{
    Menu::childClosing(result);
}
