#pragma once


#include <optional>

#include <game/graphics/menu/menu.h>


class MenuHighScore final : public Menu
{
public:
    explicit MenuHighScore();
    virtual ~MenuHighScore() = default;

    void update(const Duration &elapsed) override;
    void draw(
        sf::RenderTarget &target,
        sf::RenderStates states) const override;
    void setHighScore(unsigned int value);

private:
    void setText(unsigned int value);
    void setSecretText();
    void setup();
    void deleteHighScore();

private:
    unsigned int m_highScore{ 0 };
    Text m_text;
    std::optional<Duration> m_secretTextVisibleDuration;
};
