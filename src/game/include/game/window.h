#pragma once


#include <functional>
#include <optional>

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
    using KeyCallback = std::function<void(const sf::Event::KeyEvent&)>;

public:
    explicit Window(
        const std::string &title,
        ResizeCallback resizeCallback,
        KeyCallback keyPressCallback,
        KeyCallback keyReleaseCallback);
    ~Window();

    void beginDraw();
    void endDraw();

    void update();

    bool isDone() const noexcept;
    void close();
    bool isFullscreen();
    sf::RenderWindow& gameWindow() noexcept;
    sf::RenderWindow& debugWindow() noexcept;

    /*!
     * \param[in] size Размер в пикселях.
     */
    void resize(const sf::Vector2u &size);
    sf::Vector2u windowSize() const noexcept;
    const sf::Vector2f& viewSize() const noexcept;

    void toggleFullscreen();

    void enableDebugView(bool value);

private:
    void setup(const std::string &title);
    void create();
    void destroy();
    void onKeyPressed(const sf::Event::KeyEvent &key);

private:
    sf::RenderWindow m_window;

    /// Единицы измерения размера - пиксели игрового мира.
    sf::View m_view;
    /// Единицы измерения размера - пиксели экрана.
    std::optional<sf::View> m_viewDebug;

    sf::Vector2u m_windowSize;
    std::string m_windowTitle;
    ResizeCallback m_resizeCallback;
    KeyCallback m_keyPressCallback;
    KeyCallback m_keyReleaseCallback;
    bool m_isDone{ false };
    bool m_isFullscreen{ false };
};
