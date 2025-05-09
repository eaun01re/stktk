#pragma once


#include <SFML/Graphics.hpp>

#include <memory>

#include <game/menu/text.h>


class Number final : public sf::Drawable
{
public:
    explicit Number(unsigned int value = 1);
    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const override;
    void setPosition(const sf::Vector2f &position);
    const sf::Vector2f& position() const noexcept;
    void set(unsigned int value);

private:
    void init();
    sf::Sprite makeDigit(unsigned char digit, const sf::Vector2u &digitSize);

private:
    sf::Sprite m_frame;
    Text m_text;
};


using NumberPtr = std::shared_ptr<Number>;
