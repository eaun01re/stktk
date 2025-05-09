#pragma once


#include <SFML/Graphics.hpp>

#include "clock.h"


class Screen : public sf::Drawable
{
public:
    virtual void update(const Duration &elapsed);
    virtual bool handleKeyPressed(const sf::Keyboard::Key key) = 0;
    virtual void handleKeyReleased(const sf::Keyboard::Key key);
};
