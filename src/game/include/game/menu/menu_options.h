#pragma once


#include <game/menu/menu.h>


class MenuOptions final : public Menu
{
public:
    explicit MenuOptions(IMenuObserver *parent = nullptr);
    virtual ~MenuOptions() = default;

private:
    void setup();
    void openSubmenu();
    void onClosing() override;
    void childClosing(bool result) override;

private:
    std::shared_ptr<MenuItem> m_menuSound;
};
