#pragma once


#include <functional>
#include <game/menu/menu.h>


class MenuStart final : public Menu
{
public:
    using ClosedCallback = std::function<void(bool)>;

public:
    explicit MenuStart(const ClosedCallback &closedCallback);
    virtual ~MenuStart() = default;

    bool handleKeyPressed(const sf::Keyboard::Key key) override;
    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    void setup();
    void accept();
    void onCloseActivated() override;
    void onLeftActivated() override;
    void onRightActivated() override;

private:
    sf::Sprite m_background;
    ClosedCallback m_closedCallback;
};
