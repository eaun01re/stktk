#pragma once


#include <boost/signals2.hpp>

#include <game/consts.h>

#include <game/screen.h>
#include <game/graphics/menu/button.h>


namespace
{

const std::u32string SELECT_CAPTION = U"Выбор";

const sf::Vector2u FRAME_SIZE(
    SCREEN_SIZE.x - 7,
    SCREEN_SIZE.y - BUTTON_SIZE.y - 4);
const sf::Vector2u FRAME_POSITION(
    (SCREEN_SIZE.x - FRAME_SIZE.x) / 2 + 1,
    (SCREEN_SIZE.y - BUTTON_SIZE.y - FRAME_SIZE.y) / 2);

}


class Menu : public Screen
{
public:
    using Signal = boost::signals2::signal<void()>;
    using Slot = Signal::slot_type;

public:
    explicit Menu();
    virtual ~Menu() = default;
    bool handleKeyPressed(const sf::Keyboard::Key key) override;
    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const override;
    boost::signals2::connection connectClose(const Slot &slot);

protected:
    void makeFrame();
    void onLeftActivated();
    void onRightActivated();
    void close();
    boost::signals2::connection connectLeft(const Slot &slot);
    boost::signals2::connection connectRight(const Slot &slot);

protected:
    Button m_buttonLeft;
    Button m_buttonRight;

    Signal m_signalLeft;
    Signal m_signalRight;
    Signal m_signalClose;
    boost::signals2::connection m_connectionLeft;
    boost::signals2::connection m_connectionRight;

    std::list<sf::RectangleShape> m_frame;
};
