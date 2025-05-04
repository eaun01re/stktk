#include "start.h"

#include "resource_loader.h"


namespace
{

constexpr sf::Keyboard::Key KEY_ACCEPT = sf::Keyboard::Key::Enter;
constexpr sf::Keyboard::Key KEY_EXIT = sf::Keyboard::Key::Escape;

}


Start::Start(
    const ClosedCallback &callback,
    const std::uint8_t)
    : m_closedCallback(callback)
{
    setup();
}


void Start::update(const Duration &)
{
}


void Start::render(sf::RenderTarget &target)
{
    target.draw(m_background);
}


void Start::handleKeyPressed(const sf::Keyboard::Key key)
{
    switch (key)
    {
    case KEY_EXIT:
        m_closedCallback(std::nullopt);
        break;
    case KEY_ACCEPT:
        {
            Config config;
            config.cranesQuantity = 1;
            m_closedCallback(config);
        }
        break;
    default:
        break;
    }
}


void Start::handleKeyReleased(sf::Keyboard::Key)
{
}


void Start::setup()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_background.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Splash));
}
