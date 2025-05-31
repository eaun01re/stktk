#pragma once


#include <memory>

#include <game/screen.h>
#include <game/graphics/menu/button.h>
#include <game/graphics/menu/menu_item.h>


namespace
{

const std::u32string SELECT_CAPTION = U"Выбор";

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
        sf::RenderTarget& target,
        sf::RenderStates states) const override;
    boost::signals2::connection connectClose(const Slot &slot);

protected:
    void makeFrame();
    void moveSelection(bool down);
    void onLeftActivated();
    void onRightActivated();
    void close();
    void setItems(const std::vector<std::shared_ptr<MenuItem>> &items);
    boost::signals2::connection connectLeft(const Slot &slot);
    boost::signals2::connection connectRight(const Slot &slot);

private:
    void updateItems();
    void updateSelection();
    void clearSubmenu();
    std::size_t visibleItemIndex(std::size_t visibleIndex) const noexcept;

protected:
    Button m_buttonLeft;
    Button m_buttonRight;

    Signal m_signalLeft;
    Signal m_signalRight;
    Signal m_signalClose;
    boost::signals2::connection m_connectionLeft;
    boost::signals2::connection m_connectionRight;

    std::list<sf::RectangleShape> m_frame;

    std::vector<std::shared_ptr<MenuItem>> m_items;
    int m_currentItemIndex{ 0 };

    std::unique_ptr<sf::RectangleShape> m_selection;
    std::uint8_t m_selectionPosition{ 0 };
};
