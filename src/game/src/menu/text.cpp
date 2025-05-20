#include <game/menu/text.h>

#include <array>

#include <game/resource_loader.h>


namespace
{

/// Размерность текстуры в количестве спрайтов по горизонтали и вертикали.
const sf::Vector2u TEXTURE_SIZE(33, 5);

const sf::Vector2u CHARACTER_SIZE(7, 12);

/// Горизонтальный интервал между символами.
constexpr unsigned int CHARACTER_GAP = 1;


/// Ширины символов кириллицы в алфавитном порядке.
/// Каждый символ описан 4 значениям:
/// * широта обычного заглавного символа,
/// * широта обычного строчного символа,
/// * широта жирного заглавного символа,
/// * широта жирного строчного символа.
const std::array<std::array<unsigned char, 4>, 33> CYRILLIC_CHARACTERS_WIDTHS =
{{
    { 5, 5, 5, 5 }, // 00, а
    { 5, 5, 5, 5 }, // 01, б
    { 5, 5, 5, 5 }, // 02, в
    { 5, 5, 5, 5 }, // 03, г
    { 5, 5, 7, 6 }, // 04, д
    { 5, 5, 5, 5 }, // 05, е
    { 5, 5, 5, 5 }, // 06, ё
    { 5, 5, 7, 7 }, // 07, ж
    { 5, 5, 5, 5 }, // 08, з
    { 5, 5, 6, 6 }, // 09, и
    { 5, 5, 6, 6 }, // 10, й
    { 5, 5, 6, 5 }, // 11, к
    { 5, 5, 6, 5 }, // 12, л
    { 5, 5, 7, 7 }, // 13, м
    { 5, 5, 6, 6 }, // 14, н
    { 5, 5, 6, 5 }, // 15, о
    { 5, 5, 6, 5 }, // 16, п
    { 5, 5, 5, 5 }, // 17, р
    { 5, 5, 5, 5 }, // 18, с
    { 5, 5, 6, 6 }, // 19, т
    { 5, 5, 6, 5 }, // 20, у
    { 5, 5, 6, 6 }, // 21, ф
    { 5, 5, 5, 6 }, // 22, х
    { 5, 5, 6, 6 }, // 23, ц
    { 5, 5, 6, 5 }, // 24, ч
    { 5, 5, 7, 6 }, // 25, ш
    { 5, 5, 7, 7 }, // 26, щ
    { 5, 5, 6, 6 }, // 27, ъ
    { 5, 5, 7, 7 }, // 28, ы
    { 5, 5, 5, 5 }, // 29, ь
    { 5, 5, 5, 5 }, // 30, э
    { 5, 5, 7, 6 }, // 31, ю
    { 5, 5, 5, 5 }  // 32, я
}};

/// Ширина символов цифр в порядке возрастания.
constexpr unsigned char DIGITS_CHARACTERS_WIDTH = 5;

template<char32_t LetterA, char32_t LetterE, char32_t LetterYO>
unsigned int letterNumber(char32_t c)
{
    if (c == LetterYO)
    {
        return 15;
    }

    return c - LetterA + (c <= LetterE ? 0 : 1);
}

unsigned int upperCaseLetterNumber(char32_t c)
{
    return letterNumber<u'А', u'Е', u'Ё'>(c);
}

unsigned int lowerCaseLetterNumber(char32_t c)
{
    return letterNumber<u'а', u'е', u'ё'>(c);
}


sf::IntRect spriteRectByCharacter(char32_t c, bool bold)
{
    const unsigned int boldOffset = bold ? 1 : 0;

    sf::IntRect rect;
    if (c >= u'0' && c <= u'9')
    {
        rect = sf::IntRect(
            c - u'0' + 1,
            0,
            DIGITS_CHARACTERS_WIDTH,
            CHARACTER_SIZE.y);
    }
    if ((c >= u'А' && c <= u'Я') || c == U'Ё')
    {
        const unsigned int letterNumber = upperCaseLetterNumber(c);
        rect = sf::IntRect(
            letterNumber,
            2 + 2 * boldOffset,
            CYRILLIC_CHARACTERS_WIDTHS[letterNumber][2 * boldOffset],
            CHARACTER_SIZE.y);
    }
    if ((c >= u'а' && c <= u'я') || c == U'ё')
    {
        const unsigned int letterNumber = lowerCaseLetterNumber(c);
        rect = sf::IntRect(
            letterNumber,
            3 + 2 * boldOffset,
            CYRILLIC_CHARACTERS_WIDTHS[letterNumber][1 + 2 * boldOffset],
            CHARACTER_SIZE.y);
    }

    rect.left *= CHARACTER_SIZE.x;
    rect.top *= CHARACTER_SIZE.y;

    return rect;
}

}


Text::Text(const std::u32string &text)
{
    setText(text);
}


void Text::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::Transform transform = states.transform;
    transform.translate(m_position);
    for (const sf::Sprite &sprite : m_characters)
    {
        target.draw(sprite, transform);
    }
}


const sf::Vector2f &Text::position() const
{
    return m_position;
}


void Text::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    update();
}


void Text::setBold(bool bold)
{
    m_bold = bold;
    update();
}


const std::u32string &Text::text() const noexcept
{
    return m_text;
}


void Text::setText(const std::u32string &text)
{
    m_text = text;
    update();
}


Text::Align Text::align() const noexcept
{
    return m_align;
}


void Text::setAlign(Align align)
{
    m_align = align;
    update();
}


void Text::setColor(const sf::Color &color)
{
    m_color = color;
    update();
}


void Text::update()
{
    m_characters.clear();
    if (m_text.empty())
    {
        return;
    }

    ResourceLoader &resourceLoader = ResourceLoader::instance();
    const sf::Texture &texture =
        *resourceLoader.texture(ResourceLoader::TextureId::Font);

    int offset = 0;
    for (const char32_t c : m_text)
    {
        sf::Sprite sprite;
        sprite.setTexture(texture);
        sprite.setTextureRect(spriteRectByCharacter(c, m_bold));
        sprite.setColor(m_color);
        sprite.setPosition(offset, 0);
        m_characters.push_back(sprite);
        offset += sprite.getTextureRect().width + CHARACTER_GAP;
    }

    // Смещение для выравнивания.
    if (m_align == Align::Left)
    {
        return;
    }
    if (m_align == Align::Center)
    {
        offset /= 2;
    }
    for (sf::Sprite &sprite : m_characters)
    {
        sprite.move(-offset, 0);
    }
}
