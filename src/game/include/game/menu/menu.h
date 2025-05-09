#pragma once


#include <game/screen.h>
#include <game/menu/button.h>


class Menu : public Screen
{
public:
    explicit Menu();
    virtual ~Menu() = default;
    bool handleKeyPressed(const sf::Keyboard::Key key) override;
    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

protected:
    virtual void onCloseActivated();
    virtual void onLeftActivated();
    virtual void onRightActivated();

protected:
    Button m_buttonLeft;
    Button m_buttonRight;
};
