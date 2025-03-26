#pragma once


#include <SFML/Graphics.hpp>

namespace
{

/// Размер игрового мира. Соответствует размеру экрана телефона.
/// https://mobilespecs.net/phone/Siemens/Siemens_A52.html
const sf::Vector2u SCREEN_SIZE(101, 64);

/// Длина стороны ящика.
constexpr unsigned char BOX_SIZE = 8;

const sf::Color BACKGROUND_COLOR(217, 150, 32);

constexpr unsigned char BOXES_ROWS = 5;
constexpr unsigned char BOXES_COLUMNS = 12;

}
