#pragma once


#include <functional>
#include <list>

#include <game/menu/text.h>


namespace
{

const sf::Vector2u BUTTON_SIZE(47, 13);

}


class Button final : public sf::Drawable
{
public:
    using Callback = std::function<void()>;

public:
    explicit Button(const std::u32string &caption = std::u32string());
    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    void setAction(const Callback &callback);
    void pressed();

    const sf::Vector2f& position() const;
    void setPosition(const sf::Vector2f &position);

    void setCaption(const std::u32string &caption);

    bool enabled() const noexcept;

private:
    void update();

private:
    Callback m_callback;
    sf::Vector2f m_position;
    std::list<sf::RectangleShape> m_background;
    Text m_text;
};
