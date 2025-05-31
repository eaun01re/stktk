#include <game/graphics/score.h>

#include <game/consts.h>
#include <game/resource_loader.h>


namespace
{

constexpr unsigned int MIN_VALUE = 0;
constexpr unsigned int MAX_VALUE = 999999;

/// Положения числа относительно левого верхнего угла рамки.
const sf::Vector2f DIGITS_OFFSET(8, 2);

/// Длительность одного периода видимости текста при мигании.
constexpr Duration BLINK_DURATION = std::chrono::milliseconds(400);

}


Score::Score(unsigned int value)
{
    init();
    setValue(value);
}


void Score::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_frame, states);
    if (m_hourglass.has_value())
    {
        target.draw(m_hourglass.value(), states);
    }
    if (m_textVisible)
    {
        target.draw(m_text, states);
    }
}


void Score::update(const Duration &elapsed)
{
    if (m_blinkDuration.count() == 0)
    {
        return;
    }

    m_currentVisibleTime += elapsed;
    if (m_currentVisibleTime < m_blinkDuration)
    {
        return;
    }
    m_currentVisibleTime -= m_blinkDuration;
    m_textVisible = !m_textVisible;
}


void Score::setFramePosition(const sf::Vector2f &position)
{
    m_frame.setPosition(position);
    m_text.setPosition(position + DIGITS_OFFSET);
}


void Score::addHourglass(const sf::Vector2f &position)
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    sf::Sprite hourglass;
    hourglass.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Hourglass));
    hourglass.setColor(BACKGROUND_COLOR);
    hourglass.setPosition(position);
    m_hourglass = hourglass;
}


void Score::setValue(unsigned int value)
{
    value = std::clamp(value, MIN_VALUE, MAX_VALUE);
    const std::string valueText = std::to_string(value);
    m_text.setText(std::u32string(valueText.cbegin(), valueText.cend()));
}


void Score::enableBlinking(bool enabled)
{
    m_blinkDuration = enabled ? BLINK_DURATION : std::chrono::milliseconds(0);
}


void Score::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_frame.setTexture(
        *resourceLoader.texture(ResourceLoader::TextureId::Frame));
    m_frame.setColor(BACKGROUND_COLOR);

    m_text.setColor(TEXT_COLOR);
    m_text.setBold(true);
}
