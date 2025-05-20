#pragma once


#include <game/menu/menu.h>


class MenuLevel final : public Menu
{
public:
    explicit MenuLevel(IMenuObserver *parent = nullptr);
    virtual ~MenuLevel() = default;

private:
    void setup();
    void onClosing() override;
    std::size_t selectedItem() const noexcept;
    void setCheckedItem(std::size_t index);

private:
    std::array<std::shared_ptr<MenuItem>, 3> m_menuItems;
};
