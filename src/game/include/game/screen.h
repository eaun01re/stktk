#pragma once


#include <SFML/Graphics.hpp>

#include "clock.h"


class Screen : public sf::Drawable
{
public:
    virtual void update(const Duration &elapsed);
    /*!
     * Обрабатывает нажатие клавиши.
     * \param[in] key нажатая клавиша.
     * \return \c true, если клавиша обработана,
     * \c false - в противном случае.
     */
    virtual bool handleKeyPressed(const sf::Keyboard::Key key) = 0;
    virtual void handleKeyReleased(const sf::Keyboard::Key key);
};
