#include <game/menu/menu_item.h>

#include <game/consts.h>


namespace
{

constexpr std::size_t CHECKBOX_FRAME_SIZE = 11;
const sf::Vector2f TEXT_OFFSET(CHECKBOX_FRAME_SIZE + 3, -1);
const sf::Vector2f MARK_OFFSET(1, 5);

/*!
 * Возвращает список точек, составляющих рамку чекбокса.
 * \param[in] position Положение левого верхнего угла рамки чекбокса.
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
 * Возвращает список точек, составляющих галочку чекбокса.
 * \param[in] position Положение левого верхнего угла рамки чекбокса.
 * \return Список точек.
 */
std::list<sf::RectangleShape> makeCheckboxMark(
    const sf::Vector2f &position,
    const sf::Color &color)
{
    std::list<sf::RectangleShape> dots;
    sf::RectangleShape dot(sf::Vector2f(1, 1));
    dot.setFillColor(color);
    const sf::Vector2f offset = position + MARK_OFFSET;
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

}


MenuItem::MenuItem(
    const std::u32string &text,
    bool checkbox)
    : m_text(text)
    , m_checkbox(checkbox)
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


void MenuItem::setAction(bool left, const Action &action)
{
    (left ? m_actionLeft : m_actionRight) = action;
}


const Action& MenuItem::action(bool left) const noexcept
{
    return left ? m_actionLeft : m_actionRight;
}


bool MenuItem::checked() const noexcept
{
    return m_checked;
}


void MenuItem::setChecked(bool value)
{
    if (value == m_checked)
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
        makeCheckboxMark();
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
        sf::Vector2f(m_checkbox ? TEXT_OFFSET.x : 0, TEXT_OFFSET.y));

    if (m_checkbox)
    {
        const sf::Color &color = m_selected ? BACKGROUND_COLOR : TEXT_COLOR;
        m_checkboxFrame = makeCheckboxFrame(m_position, color);
        if (m_checked)
        {
            makeCheckboxMark();
        }
    }
}


void MenuItem::makeCheckboxMark()
{
    const sf::Color &color = m_selected ? BACKGROUND_COLOR : TEXT_COLOR;
    m_checkMark = ::makeCheckboxMark(m_position, color);
}
