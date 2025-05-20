#include <game/screen_debug.h>

#include <sstream>

#include <game/log.h>


namespace
{

// TODO: Сделать поиск шрифта.
const std::string FONT_FAMILY =
    "/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf";
const sf::Color FONT_FILL_COLOR(sf::Color::Blue);
const sf::Color FONT_OUTLINE_COLOR(sf::Color::White);
constexpr unsigned int FONT_SIZE = 14;
constexpr unsigned int FONT_OUTLINE_THICKNESS = 2;

}


ScreenDebug::ScreenDebug(const std::shared_ptr<const World> &world)
    : m_world(world)
{
    setup();
}


void ScreenDebug::update(const Duration&)
{
    if (m_world == nullptr)
    {
        return;
    }

    std::stringstream ss;
    ss << "--- Debug menu ---" << std::endl;
    ss << "System time: " << formatTimeWithMilliseconds() << std::endl;
    ss << "Score: " << m_world->m_score << std::endl;
    ss << "Cranes:" << std::endl;
    for (std::size_t i = 0; i < m_world->m_cranes.size(); ++i)
    {
        ss << ' ' << i << ": ";
        if (m_world->m_cranes[i] != nullptr)
        {
            ss  << (m_world->m_cranes[i]->isLeft() ? '<' : '>') << ' '
                << m_world->m_cranes[i]->getPosition().x;
        }
        ss << std::endl;
    }
    ss << "God mode (G): " << m_world->m_godMode << std::endl;

    m_text->setString(ss.str());
}


bool ScreenDebug::handleKeyPressed(const sf::Keyboard::Key)
{
    return false;
}


void ScreenDebug::draw(sf::RenderTarget &target, sf::RenderStates) const
{
    target.draw(*m_text);
}


void ScreenDebug::setup()
{
    m_text.reset(new sf::Text());
    m_font.reset(new sf::Font());

    if (!m_font->loadFromFile(FONT_FAMILY))
    {
        LOG_ERROR("Unable to load font \'" << FONT_FAMILY << "\'.");
        return;
    }

    m_text->setFont(*m_font);
    m_text->setCharacterSize(FONT_SIZE);
    m_text->setFillColor(FONT_FILL_COLOR);
    m_text->setOutlineColor(FONT_OUTLINE_COLOR);
    m_text->setOutlineThickness(FONT_OUTLINE_THICKNESS);
}
