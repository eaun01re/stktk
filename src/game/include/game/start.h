#pragma once


#include <functional>
#include <optional>

#include "screen.h"


class Start final : public Screen
{
public:
    struct Config
    {
        std::uint8_t cranesQuantity{ 1 };
        bool sound{ false };
    };
    using ClosedCallback = std::function<void(std::optional<Config>)>;

public:
    explicit Start(
        const ClosedCallback &callback,
        const std::uint8_t cranesQuantity = 1);
    void update(const Duration &elapsed) override;
    void handleKeyPressed(const sf::Keyboard::Key key) override;
    void handleKeyReleased(const sf::Keyboard::Key key) override;
    void render(sf::RenderTarget &target) override;

private:
    void setup();

private:
    sf::Sprite m_background;
    ClosedCallback m_closedCallback;
};
