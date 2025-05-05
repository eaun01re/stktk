#pragma once


#include <SFML/Graphics.hpp>

#include <game/objects/figure.h>


class Hourglass final : public Figure, public sf::Sprite
{
public:
    explicit Hourglass();
    void render(
        sf::RenderTarget &target,
        const sf::Transform &transform) const override;

private:
    void init();
};
