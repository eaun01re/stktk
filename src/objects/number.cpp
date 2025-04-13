#include "number.h"

#include "math.h"
#include "consts.h"
#include "resource_loader.h"


namespace
{

constexpr unsigned int MIN_VALUE = 0;
constexpr unsigned int MAX_VALUE = 999999;

/// Размерность текстуры с цифрами
/// в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_DIGITS_SIZE(1, 10);
const sf::Vector2f DIGITS_OFFSET(8, 4);

}


Number::Number(unsigned int value)
{
    init();
    set(value);
}


void Number::render(
    sf::RenderTarget &target,
    const sf::Transform &transform) const
{
    target.draw(m_frame, transform);

    sf::Transform transformDigits = transform;
    transformDigits.translate(position());
    for (const auto &sprite : m_digits)
    {
        target.draw(sprite, transformDigits);
    }
}


void Number::setPosition(const sf::Vector2f &position)
{
    m_frame.setPosition(position);
}


const sf::Vector2f& Number::position() const noexcept
{
    return m_frame.getPosition();
}


void Number::set(unsigned int value)
{
    value = std::clamp(value, MIN_VALUE, MAX_VALUE);
    m_digits.clear();

    const sf::Vector2u textureSize = m_textureDigits.getSize();
    const sf::Vector2u digitSize = sf::Vector2u(textureSize.x / 10, textureSize.y);

    // Создание спрайтов.
    do
    {
        const unsigned char digit = value % 10;
        m_digits.push_back(makeDigit(digit, digitSize));
        value /= 10;
    }
    while (value > 0);

    // Расстановка позиций.
    for (std::size_t i = 0; i < m_digits.size(); ++i)
    {
        const sf::Vector2f positionLocal(
            (m_digits.size() - 1 - i) * (digitSize.x + 1),
            0);
        m_digits[i].setPosition(sum(DIGITS_OFFSET, positionLocal));
    }
}


void Number::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    m_textureDigits = *resourceLoader.texture(ResourceLoader::TextureId::Digits);

    m_frame.setTexture(*resourceLoader.texture(ResourceLoader::TextureId::Frame));
    const sf::Vector2u textureSize = m_frame.getTexture()->getSize();
    const sf::IntRect m_currentRect(0, 0, textureSize.x, textureSize.y);
    m_frame.setTextureRect(mirrorVertical(m_currentRect));
    m_frame.setColor(BACKGROUND_COLOR);
}


sf::Sprite Number::makeDigit(unsigned char digit, const sf::Vector2u &digitSize)
{
    sf::Sprite sprite;
    if (digit > 9)
    {
        return sprite;
    }

    const sf::IntRect rect(
        digit * digitSize.x,
        0,
        digitSize.x,
        digitSize.y);
    sprite.setTexture(m_textureDigits);
    sprite.setTextureRect(mirrorVertical(rect));
    sprite.setColor(BACKGROUND_COLOR);
    return sprite;
}
