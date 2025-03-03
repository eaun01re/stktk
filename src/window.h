#pragma once


#include <functional>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


namespace
{

constexpr unsigned int FPS_LIMIT = 60;

}


class Window final
{
public:
    using ResizeCallback = std::function<void(const sf::Vector2u&)>;
    using KeyPressCallback = std::function<void(const sf::Event::KeyEvent&)>;

public:
    explicit Window(
        const std::string &title,
        ResizeCallback resizeCallback,
        KeyPressCallback keyPressCallback);
    ~Window();

    void beginDraw();
    void endDraw();

    void update();

    bool isDone() const;
    bool isFullscreen();
    sf::RenderWindow& renderWindow() noexcept;

    void resize(const sf::Vector2u &size);
    sf::Vector2u windowSize();

    void toggleFullscreen();

    void draw(sf::Drawable &l_drawable);

//    void setResizeCallback(ResizeCallback callback);

private:
    void setup(const std::string &title);
    void create();
    void destroy();

private:
    sf::RenderWindow m_window;
    sf::View m_view; ///< Единицы измерения размера - пиксели игрового мира.
    sf::Vector2u m_windowSize;
    std::string m_windowTitle;
    ResizeCallback m_resizeCallback;
    KeyPressCallback m_keyPressCallback;
    bool m_isDone{ false };
    bool m_isFullscreen{ false };
};
