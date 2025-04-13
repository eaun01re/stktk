#pragma once


#include <SFML/Graphics.hpp>

#include "objects/figure.h"


class Number final : public Figure
{
public:
    explicit Number(unsigned int value = 1);
    void render(
        sf::RenderTarget &target,
        const sf::Transform &transform) const override;
    void setPosition(const sf::Vector2f &position);
    const sf::Vector2f& position() const noexcept;
    void set(unsigned int value);

private:
    void init();
    sf::Sprite makeDigit(unsigned char digit, const sf::Vector2u &digitSize);

private:
    sf::Sprite m_frame;
    std::vector<sf::Sprite> m_digits;
    sf::Texture m_textureDigits;
};


using NumberPtr = std::shared_ptr<Number>;
