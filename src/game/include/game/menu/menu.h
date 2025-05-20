#pragma once


#include <memory>

#include <game/screen.h>
#include <game/menu/button.h>
#include <game/menu/menu_item.h>


struct IMenuObserver
{
    virtual void childClosing(bool result) = 0;
};


namespace
{

const std::u32string SELECT_CAPTION = U"Выбор";

}


class Menu : public Screen, public IMenuObserver
{
public:
    using ClosedCallback = std::function<void(bool)>;

public:
    explicit Menu(IMenuObserver *parent = nullptr);
    virtual ~Menu() = default;
    bool handleKeyPressed(const sf::Keyboard::Key key) override;
    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;
    void setSubmenu(std::unique_ptr<Menu> submenu);

protected:
    virtual void onClosing();
    void makeFrame();
    void moveSelection(bool down);
    void onLeftActivated();
    void onRightActivated();
    void close(bool result);
    void setItems(const std::vector<std::shared_ptr<MenuItem>> &items);
    void childClosing(bool argument) override;

private:
    void updateItems();
    void updateSelection();
    void clearSubmenu();
    std::size_t visibleItemIndex(std::size_t visibleIndex) const noexcept;

protected:
    Button m_buttonLeft;
    Button m_buttonRight;

    IMenuObserver *m_parent{ nullptr };

    std::unique_ptr<Menu> m_submenu;

    std::list<sf::RectangleShape> m_frame;

    std::vector<std::shared_ptr<MenuItem>> m_items;
    int m_currentItemIndex{ 0 };

    std::unique_ptr<sf::RectangleShape> m_selection;
    std::uint8_t m_selectionPosition{ 0 };
};
