#pragma once


#include <array>

#include <game/graphics/menu/menu_list.h>


class MenuLevel final : public MenuList
{
public:
    explicit MenuLevel();
    virtual ~MenuLevel() = default;

    std::size_t selectedItem() const noexcept;
    void setSelectedItem(std::size_t index);

private:
    void setup();

private:
    std::array<std::shared_ptr<MenuItem>, 3> m_menuItems;
};
