#pragma once


#include <game/menu/menu.h>


class MenuStart final : public Menu
{
public:
    explicit MenuStart();
    virtual ~MenuStart() = default;

    boost::signals2::connection connectStart(const Slot &slot);
    boost::signals2::connection connectOptions(const Slot &slot);

private:
    void setup();
};
