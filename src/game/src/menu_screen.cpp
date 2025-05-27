#include <game/menu_screen.h>

#include <game/config.h>
#include <game/log.h>
#include <game/resource_loader.h>
#include <game/sound_system.h>


MenuScreen::MenuScreen()
{
    setup();
}


void MenuScreen::draw(
    sf::RenderTarget &target,
    sf::RenderStates states) const
{
    target.draw(m_background);

    if (m_menu != nullptr)
    {
        m_menu->draw(target, states);
    }
}


bool MenuScreen::handleKeyPressed(const sf::Keyboard::Key key)
{
    if (m_menu != nullptr)
    {
        return m_menu->handleKeyPressed(key);
    }
    return false;
}


boost::signals2::connection MenuScreen::connectClose(const CloseSlot &slot)
{
    return m_signalClose.connect(slot);
}


void MenuScreen::setup()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_background.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Splash));

    setMenuStart();
}


std::unique_ptr<MenuStart> MenuScreen::makeMenuStart()
{
    std::unique_ptr<MenuStart> menuStart = std::make_unique<MenuStart>();
    menuStart->connectClose(boost::bind(&MenuScreen::exit, this));
    menuStart->connectStart(boost::bind(&MenuScreen::startGame, this));
    menuStart->connectOptions(boost::bind(&MenuScreen::setMenuOptions, this));
    return menuStart;
}


void MenuScreen::setMenuStart()
{
    LOG_DEBUG("Activating menu Start.");
    m_menu = makeMenuStart();
}


std::unique_ptr<MenuOptions> MenuScreen::makeMenuOptions()
{
    std::unique_ptr<MenuOptions> menuOptions = std::make_unique<MenuOptions>();
    menuOptions->connectClose(
        boost::bind(&MenuScreen::closeMenuOptions, this));
    menuOptions->connectStart(
        boost::bind(&MenuScreen::closeMenuOptionsAndStart, this));
    menuOptions->connectSelectLevel(
        boost::bind(&MenuScreen::setMenuSelectLevel, this));
    menuOptions->connectViewHighScore(
        boost::bind(&MenuScreen::setMenuHighScore, this));
    menuOptions->setSoundEnabled(Config::instance().sound);
    return menuOptions;
}


void MenuScreen::setMenuOptions()
{
    LOG_DEBUG("Activating menu Options.");
    m_menu = makeMenuOptions();
}


void MenuScreen::closeMenuOptions()
{
    saveSoundSetting();
    setMenuStart();
}


void MenuScreen::closeMenuOptionsAndStart()
{
    saveSoundSetting();
    startGame();
}


void MenuScreen::saveSoundSetting()
{
    MenuOptions *menuOptions = static_cast<MenuOptions*>(m_menu.get());
    LOG_DEBUG("Set sound enabled: " << menuOptions->soundEnabled() << ".");
    Config::instance().sound = menuOptions->soundEnabled();
    SoundSystem::instance().setEnabled(menuOptions->soundEnabled());
}


std::unique_ptr<MenuLevel> MenuScreen::makeMenuLevel()
{
    std::unique_ptr<MenuLevel> menuLevel = std::make_unique<MenuLevel>();
    menuLevel->connectClose(boost::bind(&MenuScreen::closeMenuLevel, this));
    menuLevel->setSelectedItem(Config::instance().cranesQuantity - 1);
    return menuLevel;
}


void MenuScreen::setMenuSelectLevel()
{
    LOG_DEBUG("Activating menu Level.");
    m_menu = makeMenuLevel();
}


void MenuScreen::closeMenuLevel()
{
    MenuLevel *menuLevel = static_cast<MenuLevel*>(m_menu.get());
    Config::instance().cranesQuantity = menuLevel->selectedItem() + 1;
    LOG_DEBUG("Set level: " << int(Config::instance().cranesQuantity) << ".");

    setMenuOptions();
}


void MenuScreen::setMenuHighScore()
{
    LOG_DEBUG("Activating menu High score.");
    LOG_WARNING("TODO: Viewing high score is not implemented yet.");
}


void MenuScreen::startGame()
{
    LOG_DEBUG("Start game requested.");
    m_signalClose(true);
}


void MenuScreen::exit()
{
    LOG_DEBUG("Exit requested.");
    m_signalClose(false);
}
