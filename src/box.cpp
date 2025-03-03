#include "box.h"

#include <cmath>

#include "math.h"
#include "player.h"


// unsigned int Box::boxTextureVariants()
// {
//     return m_boxTextureVariants;
// }


// unsigned int Box::m_boxTextureVariants = 1;
// unsigned int Box::m_boxTextureVariants = 1;


Box::Box()
{
    m_sprite.setPosition(0, 0);
}


void Box::init(const sf::Texture &texture, unsigned int style, const sf::Vector2f &position)
{
    m_sprite.setTexture(texture);
    m_sprite.setTextureRect(mirrorVertical(sf::IntRect(style * BOX_SIZE, 0, BOX_SIZE, BOX_SIZE)));
    m_sprite.setPosition(position);

    // m_boxTextureVariants = texture.getSize().x / BOX_SIZE;
    // m_distributionBoxStyle = std::uniform_int_distribution<std::mt19937::result_type>(0, m_boxTextureVariants - 1);
}


sf::Vector2f Box::position() const
{
    return m_sprite.getPosition();
}


void Box::move(Direction direction)
{
    switch (direction)
    {
    case Direction::Left:
        m_speed = sf::Vector2f(-SPEED_HORIZONTAL, 0);
        m_movementLength = sf::Vector2f(-BOX_SIZE, 0);
        break;
    case Direction::Right:
        m_speed = sf::Vector2f(SPEED_HORIZONTAL, 0);
        m_movementLength = sf::Vector2f(BOX_SIZE, 0);
        break;
    case Direction::Down:
        m_speed = sf::Vector2f(0, SPEED_VECTICAL);
        m_movementLength = sf::Vector2f(0, BOX_SIZE);
        break;
    }
}


void Box::fall()
{
    m_speed.y = -SPEED_VECTICAL;
}


void Box::normalizePosition(bool horizontal, bool vertical)
{
    const sf::Vector2f newPosition = normalizedPosition(position(), horizontal, vertical);
    m_sprite.setPosition(newPosition);
}



void Box::update(const Duration &elapsed)
{
    if (!isTolerant(m_speed))
    {
        return;
    }

    // const double elapsedSeconds = elapsed.count();
    // const sf::Vector2f offset(
    //     m_speed.value().x * elapsedSeconds,
    //     m_speed.value().y * elapsedSeconds);
    // m_sprite.move(offset.x, offset.y);

    const double elapsedSeconds = elapsed.count();
    const sf::Vector2f offset(m_speed.x * elapsedSeconds, m_speed.y * elapsedSeconds);
    m_sprite.move(offset.x, offset.y);

    // Сокращение "длины" горзинтальной и вертикальной составляющих целевого
    // вектора смещения (m_movementLength), пока это сокращение не
    // приведет к смене знака у составляющей.
    if (std::isless(m_movementLength.x, 0.0f) == std::isless(m_movementLength.x - offset.x, 0.0f))
    {
        // После вычитания смещения из вектора m_movementLength в нём ещё остаётся .
        m_movementLength.x -= offset.x;
    }
    else
    {
        m_speed.x = 0;
        normalizePosition(true, false);
        m_movementLength.x = 0;
    }

    if (std::isless(m_movementLength.y, 0.0f) == std::isless(m_movementLength.y - offset.y, 0.0f))
    {
        m_movementLength.y -= offset.y;// копится значение пока игрок стоит!
    }
    else
    {
        // m_speed.y = 0;
        m_movementLength.y = 0;
        normalizePosition(false, true);
        fall();
    }
}


void Box::render(sf::RenderTarget &target, const sf::Transform &transform) const
{
    target.draw(m_sprite, transform);
}
