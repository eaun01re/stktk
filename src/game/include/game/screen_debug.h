#pragma once


#include <SFML/Graphics.hpp>

#include <game/screen.h>
#include <game/world.h>


class ScreenDebug final : public Screen
{
public:
    explicit ScreenDebug(const std::shared_ptr<const World> &world);
    void update(const Duration &elapsed) override;
    bool handleKeyPressed(const sf::Keyboard::Key key) override;
    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    void setup();

private:
    // NOTE: Без указателей объекты sf::Font и sf::Text портятся
    // при копировании экземпляра содержащего их класса.
    std::unique_ptr<sf::Font> m_font;
    std::unique_ptr<sf::Text> m_text;
    std::shared_ptr<const World> m_world;
};
