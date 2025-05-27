#pragma once


#include <array>

#include <game/menu/menu.h>


class MenuLevel final : public Menu
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
