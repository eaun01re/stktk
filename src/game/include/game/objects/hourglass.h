#pragma once


#include <SFML/Graphics.hpp>


class Hourglass final : public sf::Sprite
{
public:
    explicit Hourglass();

private:
    void init();
};
