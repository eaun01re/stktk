#pragma once


#include <SFML/Graphics.hpp>

#include "clock.h"


class Screen
{
public:
    virtual void update(const Duration &elapsed) = 0;
    virtual void render(sf::RenderTarget &target) = 0;
    virtual void handleKeyPressed(const sf::Keyboard::Key key) = 0;
    virtual void handleKeyReleased(const sf::Keyboard::Key key) = 0;
};
