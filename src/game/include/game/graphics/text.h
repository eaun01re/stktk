#pragma once


#include <SFML/Graphics.hpp>


// sf::Font: Abstraction and Bitmap Fonts
// https://en.sfml-dev.org/forums/index.php?topic=21787.0

class Text final : public sf::Drawable
{
public:
    enum Align
    {
        Left,
        Center,
        Right
    };

public:
    explicit Text(const std::u32string &text = std::u32string());
    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    const sf::Vector2f& position() const;
    void setPosition(const sf::Vector2f &position);

    bool bold() const noexcept;
    void setBold(bool bold);

    const std::u32string& text() const noexcept;
    void setText(const std::u32string &text);

    Align align() const noexcept;
    void setAlign(Align align);

    void setColor(const sf::Color &color);

private:
    void update();

private:
    std::u32string m_text;
    sf::Vector2f m_position;
    Align m_align{ Align::Left };
    bool m_bold{ false };
    sf::Color m_color;
    std::vector<sf::Sprite> m_characters;
};
