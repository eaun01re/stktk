#include <game/objects/number.h>

#include <game/consts.h>
#include <game/resource_loader.h>


namespace
{

constexpr unsigned int MIN_VALUE = 0;
constexpr unsigned int MAX_VALUE = 999999;

/// Положения числа относительно левого верхнего угла рамки.
const sf::Vector2f DIGITS_OFFSET(8, 2);

}


Number::Number(unsigned int value)
{
    init();
    set(value);
}


void Number::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_frame, states);
    target.draw(m_text, states);
}


void Number::setPosition(const sf::Vector2f &position)
{
    m_frame.setPosition(position);
    m_text.setPosition(position + DIGITS_OFFSET);
}


const sf::Vector2f& Number::position() const noexcept
{
    return m_frame.getPosition();
}


void Number::set(unsigned int value)
{
    value = std::clamp(value, MIN_VALUE, MAX_VALUE);
    const std::string valueText = std::to_string(value);
    m_text.setText(std::u32string(valueText.cbegin(), valueText.cend()));
}


void Number::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_frame.setTexture(*resourceLoader.texture(ResourceLoader::TextureId::Frame));
    m_frame.setColor(BACKGROUND_COLOR);
    m_text.setColor(TEXT_COLOR);
}
