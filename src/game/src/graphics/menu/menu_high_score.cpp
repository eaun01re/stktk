#include <game/graphics/menu/menu_high_score.h>


namespace
{

constexpr float TEXT_VERTICAL_POSITION = 18;
/// Длительность отображения секретного текста.
const Duration SECRET_TEXT_VISIBLE_DURATION = std::chrono::seconds(1);

}


MenuHighScore::MenuHighScore()
{
    setup();
}


void MenuHighScore::update(const Duration &elapsed)
{
    if (!m_secretTextVisibleDuration.has_value())
    {
        return;
    }

    m_secretTextVisibleDuration.value() += elapsed;
    if (m_secretTextVisibleDuration > SECRET_TEXT_VISIBLE_DURATION)
    {
        setText(m_highScore);
        m_secretTextVisibleDuration = std::nullopt;
    }
}


void MenuHighScore::draw(
    sf::RenderTarget &target,
    sf::RenderStates states) const
{
    Menu::draw(target, states);
    target.draw(m_text);
}


unsigned int MenuHighScore::highScore() const noexcept
{
    return m_highScore;
}


void MenuHighScore::setHighScore(unsigned int value)
{
    m_highScore = value;

    if (m_highScore > 9000)
    {
        setSecretText();
    }
    else
    {
        setText(m_highScore);
    }

    if (m_highScore > 0)
    {
        m_buttonLeft.setCaption(U"Удалить");
        connectLeft([this](){ deleteHighScore(); });
    }
}


void MenuHighScore::setText(unsigned int value)
{
    const std::string valueText = std::to_string(value);
    m_text.setText(
        std::u32string(valueText.cbegin(), valueText.cend()) + U" очков");
}


void MenuHighScore::setSecretText()
{
    m_text.setText(U"Более 9000 очков");
    m_secretTextVisibleDuration = Duration();
}


void MenuHighScore::setup()
{
    makeFrame();

    m_buttonRight.setCaption(U"ОК");
    connectRight(boost::bind(&MenuHighScore::close, this));

    m_text.setPosition(sf::Vector2f(SCREEN_SIZE.x / 2, TEXT_VERTICAL_POSITION));
    m_text.setAlign(Text::Align::Center);
    m_text.setBold(true);
    setText(m_highScore);
}


void MenuHighScore::deleteHighScore()
{
    m_highScore = 0;
    setText(m_highScore);
    m_buttonLeft.setCaption(U"");
    m_connectionLeft.disconnect();
}
