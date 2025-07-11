#include <game/window.h>

#include <game/consts.h>


namespace
{

const float ASPECT_RATIO = float(SCREEN_SIZE.x) / SCREEN_SIZE.y;

constexpr int SCREEN_SIZE_MULTIPLIER = 8;

const sf::Color CLIP_COLOR = sf::Color::Black;

}


Window::Window(
    const std::string &title,
    ResizeCallback resizeCallback,
    KeyCallback keyPressCallback,
    KeyCallback keyReleaseCallback)
    : m_resizeCallback(resizeCallback)
    , m_keyPressCallback(keyPressCallback)
    , m_keyReleaseCallback(keyReleaseCallback)
{
    setup(title);
}


Window::~Window()
{
    close();
}


void Window::setup(const std::string &title)
{
    m_windowTitle = title;
    m_windowSize = sf::Vector2u(
        SCREEN_SIZE.x * SCREEN_SIZE_MULTIPLIER,
        SCREEN_SIZE.y * SCREEN_SIZE_MULTIPLIER);
    create();

    m_view = sf::View(sf::FloatRect(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y));

    m_mask1.setFillColor(CLIP_COLOR);
    m_mask2.setFillColor(CLIP_COLOR);
}


void Window::create()
{
    const sf::Uint32 style = m_isFullscreen
        ? sf::Style::Fullscreen
        : sf::Style::Resize | sf::Style::Close;
    // FIXME: Падает при Release With Debug Info сборке на Windows.
    m_window.create(
        sf::VideoMode(m_windowSize.x, m_windowSize.y),
        m_windowTitle,
        style);
    m_window.setFramerateLimit(FPS_LIMIT);
}


void Window::destroy()
{
    m_window.close();
}


void Window::onKeyPressed(const sf::Event::KeyEvent &key)
{
    if (key.code == sf::Keyboard::F5)
    {
        toggleFullscreen();
        return;
    }

    m_keyPressCallback(key);
}


void Window::updateClippingMask(const float aspectRatio)
{
    const sf::Vector2f &viewSize = m_view.getSize();

    sf::Vector2f maskSize;
    if (aspectRatio > ASPECT_RATIO)
    {
        // Ширина больше. Маски слева и справа.
        maskSize = sf::Vector2f((viewSize.x - SCREEN_SIZE.x) / 2.0, viewSize.y);
        m_mask1.setPosition(-maskSize.x, 0);
        m_mask2.setPosition(float(SCREEN_SIZE.x), 0);
    }
    else
    {
        // Высота больше. Маски сверху и снизу.
        maskSize = sf::Vector2f(viewSize.x, (viewSize.y - SCREEN_SIZE.y) / 2.0);
        m_mask1.setPosition(0, -maskSize.y);
        m_mask2.setPosition(0, float(SCREEN_SIZE.y));
    }
    m_mask1.setSize(maskSize);
    m_mask2.setSize(maskSize);
}


void Window::beginDraw()
{
    m_window.clear(BACKGROUND_COLOR);
}


void Window::endDraw()
{
    m_window.display();
}


bool Window::isDone() const noexcept
{
    return m_isDone;
}


void Window::close()
{
    destroy();
    m_isDone = true;
}


bool Window::isFullscreen()
{
    return m_isFullscreen;
}


sf::RenderWindow& Window::gameWindow() noexcept
{
    m_window.setView(m_view);
    return m_window;
}


sf::RenderWindow& Window::debugWindow() noexcept
{
    if (m_viewDebug.has_value())
    {
        m_window.setView(m_viewDebug.value());
    }
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

    updateClippingMask(aspectRatio);

    if (m_viewDebug.has_value())
    {
        m_viewDebug = sf::View(sf::FloatRect(0, 0, size.x, size.y));
    }
}


sf::Vector2u Window::windowSize() const noexcept
{
    return m_windowSize;
}


const sf::Vector2f& Window::viewSize() const noexcept
{
    return m_view.getSize();
}


void Window::renderClippingMask(sf::RenderWindow &target)
{
    target.draw(m_mask1);
    target.draw(m_mask2);
}


void Window::toggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    destroy();
    create();
}


void Window::enableDebugView(bool value)
{
    if (!value)
    {
        m_viewDebug = std::nullopt;
        return;
    }

    const sf::Vector2u windowSize = m_window.getSize();
    m_viewDebug = sf::View(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
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
            onKeyPressed(event.key);
            break;
        case sf::Event::KeyReleased:
            if (m_keyReleaseCallback)
            {
                m_keyReleaseCallback(event.key);
            }
            break;
        default:
            break;
        }
    }
}
