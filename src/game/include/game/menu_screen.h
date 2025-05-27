#pragma once


#include <game/menu/menu_level.h>
#include <game/menu/menu_start.h>
#include <game/menu/menu_options.h>


class MenuScreen final : public Screen
{
public:
    using CloseSignal = boost::signals2::signal<void(bool)>;
    using CloseSlot = CloseSignal::slot_type;

public:
    explicit MenuScreen();
    virtual ~MenuScreen() = default;

    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const override;
    bool handleKeyPressed(const sf::Keyboard::Key key) override;

    boost::signals2::connection connectClose(const CloseSlot &slot);

private:
    void setup();

    std::unique_ptr<MenuStart> makeMenuStart();
    void setMenuStart();

    std::unique_ptr<MenuOptions> makeMenuOptions();
    void setMenuOptions();
    void closeMenuOptions();
    void closeMenuOptionsAndStart();
    void saveSoundSetting();

    std::unique_ptr<MenuLevel> makeMenuLevel();
    void setMenuSelectLevel();
    void closeMenuLevel();

    void setMenuHighScore();

    void startGame();
    void exit();

private:
    sf::Sprite m_background;
    CloseSignal m_signalClose;
    std::unique_ptr<Menu> m_menu;
};
