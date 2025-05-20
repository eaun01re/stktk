#pragma once


#include <functional>
#include <list>

#include <game/menu/text.h>


struct Action
{
    using Callback = std::function<void()>;

    std::u32string caption;
    Callback action;
};


class MenuItem final : public sf::Drawable
{
public:
    using Callback = std::function<void()>;

public:
    explicit MenuItem(const std::u32string &text, bool checkbox);

    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    void setAction(bool left, const Action &action);
    const Action& action(bool left) const noexcept;

    bool checked() const noexcept;
    void setChecked(bool value);

    void setPosition(const sf::Vector2f &position);
    void setSelected(bool value);

private:
    void update();
    void makeCheckboxMark();

private:
    Text m_text;
    bool m_checkbox{ false };
    bool m_checked{ false };
    bool m_selected{ false };
    sf::Vector2f m_position;
    std::list<sf::RectangleShape> m_checkboxFrame;
    std::list<sf::RectangleShape> m_checkMark;
    Action m_actionLeft;
    Action m_actionRight;
};
