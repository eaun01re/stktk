#pragma once


#include <game/graphics/menu/menu.h>


class MenuOptions final : public Menu
{
public:
    explicit MenuOptions();
    virtual ~MenuOptions() = default;

    boost::signals2::connection connectStart(const Slot &slot);
    boost::signals2::connection connectSelectLevel(const Slot &slot);
    boost::signals2::connection connectViewHighScore(const Slot &slot);

    bool soundEnabled() const noexcept;
    void setSoundEnabled(bool value);

private:
    void setup();

private:
    std::shared_ptr<MenuItem> m_menuItemStart;
    std::shared_ptr<MenuItem> m_menuItemSelectLevel;
    std::shared_ptr<MenuItem> m_menuHighScore;
    std::shared_ptr<MenuItem> m_menuItemSound;
};
