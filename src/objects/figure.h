#pragma once


#include <memory>

#include <SFML/Graphics.hpp>


class Figure
{
public:
    virtual void render(
        sf::RenderTarget &target,
        const sf::Transform &transform) const = 0;
};


using FigurePtr = std::shared_ptr<Figure>;
