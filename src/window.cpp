#include "window.h"


namespace
{

const sf::Color BACKGROUND_COLOR(217, 150, 32);
/// Размер игрового мира. Соответствует размеру экрана телефона.
// https://mobilespecs.net/phone/Siemens/Siemens_A52.html
const sf::Vector2u SCREEN_SIZE(101, 64);
const float ASPECT_RATIO = float(SCREEN_SIZE.x) / SCREEN_SIZE.y;
constexpr int SCREEN_SIZE_MULTIPLIER = 8;

}


Window::Window(
    const std::string &title,
    ResizeCallback resizeCallback,
    KeyPressCallback keyPressCallback)
    : m_resizeCallback(resizeCallback)
    , m_keyPressCallback(keyPressCallback)
{
    setup(title);
}


Window::~Window()
{
    destroy();
}


void Window::setup(const std::string &title)
{
    m_windowTitle = title;
    m_windowSize = sf::Vector2u(
        SCREEN_SIZE.x * SCREEN_SIZE_MULTIPLIER,
        SCREEN_SIZE.y * SCREEN_SIZE_MULTIPLIER);
    create();

    m_view = sf::View(sf::FloatRect(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y));
    m_window.setView(m_view);
}


void Window::create()
{
    const sf::Uint32 style = m_isFullscreen
        ? sf::Style::Fullscreen
        : sf::Style::Resize | sf::Style::Close;
    m_window.create(
        sf::VideoMode(m_windowSize.x, m_windowSize.y, 32),
        m_windowTitle,
        style);
    m_window.setFramerateLimit(FPS_LIMIT);
}


void Window::destroy()
{
    m_window.close();
}


void Window::beginDraw()
{
    m_window.clear(BACKGROUND_COLOR);
}


void Window::endDraw()
{
    m_window.display();
}


bool Window::isDone() const
{
    return m_isDone;
}


bool Window::isFullscreen()
{
    return m_isFullscreen;
}


sf::RenderWindow& Window::renderWindow() noexcept
{
    return m_window;
}


void Window::resize(const sf::Vector2u &size)
{
    // Определение количества дополнительных пикселей игрового мира,
    // которые нужно отобразить, учитывая размер окна в пикселях монитора
    // и соотношение сторон окна.
    // SFML 2.4 For Beginners - 11: Views.
    // https://www.youtube.com/watch?v=CpVbMeYryKo
    const float aspectRatio = float(size.x) / float(size.y);
    sf::Vector2u viewSize = SCREEN_SIZE;
    if (aspectRatio > ASPECT_RATIO)
    {
        // Ширина больше.
        viewSize.x = size.x * SCREEN_SIZE.x / (size.y * ASPECT_RATIO);
    }
    else
    {
        // Высота больше.
        viewSize.y = size.y * SCREEN_SIZE.y / (size.x / ASPECT_RATIO);
    }
    m_view.setSize(viewSize.x, viewSize.y);
    m_window.setView(m_view);
}


void Window::draw(sf::Drawable &l_drawable)
{
    m_window.draw(l_drawable);
}


//void Window::setResizeCallback(ResizeCallback callback)
//{
//    m_resizeCallback = std::move(callback);
//}


sf::Vector2u Window::windowSize()
{
    return m_windowSize;
}


void Window::toggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    destroy();
    create();
}


void Window::update()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            m_isDone = true;
            break;
        case sf::Event::Resized:
            if (m_resizeCallback)
            {
                m_resizeCallback(m_window.getSize());
            }
            break;
        case sf::Event::KeyPressed:
            if (m_keyPressCallback)
            {
                m_keyPressCallback(event.key);
            }
//            if (event.key.code == sf::Keyboard::F5)
//            {
//                toggleFullscreen();
//            }
            break;
        default:
            break;
        }
    }
}
