#pragma once


#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>

#include <game/clock.h>
#include <game/graphics/text.h>


class Score final : public sf::Drawable
{
public:
    explicit Score(unsigned int value);
    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const override;
    void update(const Duration &elapsed);
    void setFramePosition(const sf::Vector2f &position);
    void addHourglass(const sf::Vector2f &position);
    void setValue(unsigned int value);
    void enableBlinking(bool enabled);

private:
    void init();

private:
    sf::Sprite m_frame;
    std::optional<sf::Sprite> m_hourglass;
    Text m_text;
    Duration m_blinkDuration{ 0 };
    Duration m_currentVisibleTime{ 0 };
    bool m_textVisible{ true };
};


using ScorePtr = std::shared_ptr<Score>;
