#pragma once


#include <game/menu/menu.h>


class MenuStart final : public Menu
{
public:
    explicit MenuStart(IMenuObserver *parent = nullptr);
    virtual ~MenuStart() = default;

    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    void setup();
    void openSubmenu();
    void childClosing(bool result) override;

private:
    sf::Sprite m_background;
};
