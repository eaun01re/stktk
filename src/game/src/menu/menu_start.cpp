#include <game/menu/menu_start.h>

#include <game/config.h>
#include <game/resource_loader.h>


MenuStart::MenuStart(const ClosedCallback &closedCallback)
    : m_closedCallback(closedCallback)
{
    setup();
}


bool MenuStart::handleKeyPressed(const sf::Keyboard::Key key)
{
    return Menu::handleKeyPressed(key);
}


void MenuStart::draw(
    sf::RenderTarget &target,
    sf::RenderStates states) const
{
    Menu::draw(target, states);
    target.draw(m_background);
}


void MenuStart::setup()
{
    m_buttonLeft.setCaption(U"Начать");
    m_buttonRight.setCaption(U"Опции");

    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_background.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Splash));
}


void MenuStart::accept()
{
    Config &config = Config::instance();
    config.cranesQuantity = 1;
    config.sound = true;
    m_closedCallback(true);
}


void MenuStart::onCloseActivated()
{
    m_closedCallback(false);
}


void MenuStart::onLeftActivated()
{
    accept();
}


void MenuStart::onRightActivated()
{
}
