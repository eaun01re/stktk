#include <game/graphics/menu/menu_item.h>

#include <game/consts.h>


namespace
{

constexpr std::size_t CHECKBOX_FRAME_SIZE = 11;
const sf::Vector2f TEXT_OFFSET(CHECKBOX_FRAME_SIZE + 3, -1);

/*!
 * Возвращает список точек, составляющих квадратную рамку чекбокса.
 * \param[in] position Положение левого верхнего угла рамки чекбокса.
 * \param[in] color Цвет рамки.
 * \return Список точек.
 */
std::list<sf::RectangleShape> makeCheckboxFrame(
    const sf::Vector2f &position,
    const sf::Color &color)
{
    std::list<sf::RectangleShape> dots;
    sf::RectangleShape dot(sf::Vector2f(1, 1));
    dot.setFillColor(color);
    const float size = CHECKBOX_FRAME_SIZE - 1;
    for (std::size_t i = 0; i < CHECKBOX_FRAME_SIZE / 2; ++i)
    {
        dot.setPosition(position + sf::Vector2f(i * 2, 0));
        dots.push_back(dot);
        dot.setPosition(position + sf::Vector2f(size, i * 2));
        dots.push_back(dot);
        dot.setPosition(position + sf::Vector2f(size - i * 2, size));
        dots.push_back(dot);
        dot.setPosition(position + sf::Vector2f(0, size - i * 2));
        dots.push_back(dot);
    }
    return dots;
}

/*!
 * Возвращает список точек, составляющих круглую рамку радио-кнопки.
 * \param[in] position Положение левого верхнего угла рамки радио-кнопки.
 * \param[in] color Цвет рамки.
 * \return Список точек.
 */
std::list<sf::RectangleShape> makeRadioButtonFrame(
    const sf::Vector2f &position,
    const sf::Color &color)
{
    std::list<sf::RectangleShape> dots;
    sf::RectangleShape dot(sf::Vector2f(1, 1));
    dot.setFillColor(color);

    dot.setPosition(position + sf::Vector2f(0, 4));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(1, 2));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(2, 1));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(4, 0));
    dots.push_back(dot);

    dot.setPosition(position + sf::Vector2f(6, 0));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(8, 1));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(9, 2));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(10, 4));
    dots.push_back(dot);

    dot.setPosition(position + sf::Vector2f(0, 6));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(1, 8));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(2, 9));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(4, 10));
    dots.push_back(dot);

    dot.setPosition(position + sf::Vector2f(6, 10));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(8, 9));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(9, 8));
    dots.push_back(dot);
    dot.setPosition(position + sf::Vector2f(10, 6));
    dots.push_back(dot);

    return dots;
}

/*!
 * Возвращает список точек, составляющих галочку чекбокса.
 * \param[in] position Положение левого верхнего угла рамки чекбокса.
 * \param[in] color Цвет галочки.
 * \return Список точек.
 */
std::list<sf::RectangleShape> makeCheckMark(
    const sf::Vector2f &position,
    const sf::Color &color)
{
    std::list<sf::RectangleShape> dots;
    sf::RectangleShape dot(sf::Vector2f(1, 1));
    dot.setFillColor(color);
    const sf::Vector2f offset = position + sf::Vector2f(1, 5);
    dot.setPosition(offset);
    dots.push_back(dot);
    dot.setPosition(offset + sf::Vector2f(0, 1));
    dots.push_back(dot);
    dot.setPosition(offset + sf::Vector2f(1, 0));
    dots.push_back(dot);
    dot.setPosition(offset + sf::Vector2f(1, 1));
    dots.push_back(dot);
    dot.setPosition(offset + sf::Vector2f(1, 2));
    dots.push_back(dot);
    dot.setPosition(offset + sf::Vector2f(2, 1));
    dots.push_back(dot);
    dot.setPosition(offset + sf::Vector2f(2, 2));
    dots.push_back(dot);
    dot.setPosition(offset + sf::Vector2f(2, 3));
    dots.push_back(dot);
    for (int i = 0; i < 5; ++i)
    {
        dot.setPosition(offset + sf::Vector2f(3 + i, 1 - i));
        dots.push_back(dot);
        dot.setPosition(offset + sf::Vector2f(3 + i, 2 - i));
        dots.push_back(dot);
        dot.setPosition(offset + sf::Vector2f(3 + i, 3 - i));
        dots.push_back(dot);
    }
    return dots;
}

/*!
 * Возвращает список точек, составляющих кружок радио-кнопки.
 * \param[in] position Положение левого верхнего угла рамки радио-кнопки.
 * \param[in] color Цвет кружка.
 * \return Список точек.
 */
std::list<sf::RectangleShape> makeRadioMark(
    const sf::Vector2f &position,
    const sf::Color &color)
{
    std::list<sf::RectangleShape> dots;
    sf::RectangleShape dot(sf::Vector2f(1, 1));
    dot.setFillColor(color);

    const sf::Vector2f offset = position + sf::Vector2f(4, 2);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3 + 2 * i; ++j)
        {
            dot.setPosition(offset + sf::Vector2f(-i + j, i));
            dots.push_back(dot);
            dot.setPosition(offset + sf::Vector2f(-i + j, 6 - i));
            dots.push_back(dot);
        }
    }
    for (int i = 0; i < 7; ++i)
    {
        dot.setPosition(offset + sf::Vector2f(i - 2, 3));
        dots.push_back(dot);
    }

    return dots;
}

}


MenuItem::MenuItem(
    const std::u32string &text,
    Type type)
    : m_text(text)
    , m_type(type)
{
    update();
}


void MenuItem::draw(sf::RenderTarget &target, sf::RenderStates) const
{
    for (const sf::RectangleShape &dot : m_checkboxFrame)
    {
        target.draw(dot);
    }
    for (const sf::RectangleShape &dot : m_checkMark)
    {
        target.draw(dot);
    }
    target.draw(m_text);
}


Action& MenuItem::action(bool left)
{
    return left ? m_actionLeft : m_actionRight;
}


bool MenuItem::checked() const noexcept
{
    return m_checked;
}


void MenuItem::setChecked(bool value)
{
    if (m_type == Type::Simple || m_checked == value)
    {
        return;
    }

    m_checked = value;
    if (!m_checked)
    {
        m_checkMark.clear();
    }
    else
    {
        makeCheckMark();
    }
}


void MenuItem::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    update();
}


void MenuItem::setSelected(bool value)
{
    if (m_selected == value)
    {
        return;
    }

    m_selected = value;
    const sf::Color &color = m_selected ? BACKGROUND_COLOR : TEXT_COLOR;
    m_text.setColor(color);
    m_text.setBold(m_selected);
    for (sf::RectangleShape &dot : m_checkboxFrame)
    {
        dot.setFillColor(color);
    }
    for (sf::RectangleShape &dot : m_checkMark)
    {
        dot.setFillColor(color);
    }
}


void MenuItem::update()
{
    m_text.setPosition(
        m_position +
        sf::Vector2f(
            m_type == Type::Simple ? 0 : TEXT_OFFSET.x,
            TEXT_OFFSET.y));

    if (m_type == Type::Simple)
    {
        return;
    }
    const sf::Color &color = m_selected ? BACKGROUND_COLOR : TEXT_COLOR;
    switch (m_type)
    {
    case Type::CheckBox:
        m_checkboxFrame = makeCheckboxFrame(m_position, color);
        break;
    case Type::RadioButton:
        m_checkboxFrame = makeRadioButtonFrame(m_position, color);
        break;
    default:
        break;
    }

    if (m_checked)
    {
        makeCheckMark();
    }
}


void MenuItem::makeCheckMark()
{
    const sf::Color &color = m_selected ? BACKGROUND_COLOR : TEXT_COLOR;
    switch (m_type)
    {
    case Type::CheckBox:
        m_checkMark = ::makeCheckMark(m_position, color);
        break;
    case Type::RadioButton:
        m_checkMark = ::makeRadioMark(m_position, color);
        break;
    default:
        break;
    }
}
