#pragma once


#include <memory>

#include <game/graphics/menu/menu.h>
#include <game/graphics/menu/menu_item.h>


class MenuList : public Menu
{
public:
    explicit MenuList() = default;
    virtual ~MenuList() = default;
    bool handleKeyPressed(const sf::Keyboard::Key key) override;
    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const override;

protected:
    void moveSelection(bool down);
    void close();
    void setItems(const std::vector<std::shared_ptr<MenuItem>> &items);

private:
    void updateItems();
    void updateSelection();
    std::size_t visibleItemIndex(std::size_t visibleIndex) const noexcept;

protected:
    std::vector<std::shared_ptr<MenuItem>> m_items;
    int m_currentItemIndex{ 0 };

    std::unique_ptr<sf::RectangleShape> m_selection;
    std::uint8_t m_selectionPosition{ 0 };
};
