#include <game/menu/menu_start.h>

#include <game/menu/menu_options.h>
#include <game/resource_loader.h>


MenuStart::MenuStart(IMenuObserver *parent)
    : Menu(parent)
{
    setup();
}


void MenuStart::draw(
    sf::RenderTarget &target,
    sf::RenderStates states) const
{
    target.draw(m_background);
    Menu::draw(target, states);
}


void MenuStart::setup()
{
    m_buttonLeft.setCaption(U"Начать");
    m_buttonLeft.setAction([this](){ close(true); });
    m_buttonRight.setCaption(U"Опции");
    m_buttonRight.setAction(std::bind(&MenuStart::openSubmenu, this));

    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_background.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Splash));
}


void MenuStart::openSubmenu()
{
    m_submenu.reset(new MenuOptions(this));
}


void MenuStart::childClosing(bool result)
{
    if (result)
    {
        close(true);
    }
    Menu::childClosing(result);
}
