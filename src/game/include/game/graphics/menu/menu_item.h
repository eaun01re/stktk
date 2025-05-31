#pragma once


#include <cstdint>
#include <list>

#include <boost/signals2.hpp>

#include <game/graphics/text.h>


struct Action
{
    using Signal = boost::signals2::signal<void()>;

    std::u32string caption;
    Signal signal;
};


class MenuItem final : public sf::Drawable
{
public:
    enum Type : std::uint8_t
    {
        Simple,
        CheckBox,
        RadioButton
    };

public:
    explicit MenuItem(
        const std::u32string &text,
        Type type = MenuItem::Type::Simple);

    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    void setAction(bool left, const Action &action);
    Action& action(bool left);

    bool checked() const noexcept;
    void setChecked(bool value);

    void setPosition(const sf::Vector2f &position);
    void setSelected(bool value);

private:
    void update();
    void makeCheckMark();

private:
    Text m_text;
    const Type m_type{ Type::Simple };
    bool m_checked{ false };
    bool m_selected{ false };
    sf::Vector2f m_position;
    std::list<sf::RectangleShape> m_checkboxFrame;
    std::list<sf::RectangleShape> m_checkMark;
    Action m_actionLeft;
    Action m_actionRight;
};
