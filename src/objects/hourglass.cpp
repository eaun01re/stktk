#include "hourglass.h"

#include "consts.h"
#include "resource_loader.h"
#include "math/math.h"


Hourglass::Hourglass()
{
    init();
}


void Hourglass::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    setTexture(*resourceLoader.texture(ResourceLoader::TextureId::Hourglass));
    const sf::Vector2u textureSize = getTexture()->getSize();
    const sf::IntRect m_currentRect(0, 0, textureSize.x, textureSize.y);
    setTextureRect(mirrorVertical(m_currentRect));
    setColor(BACKGROUND_COLOR);
}


void Hourglass::render(
    sf::RenderTarget &target,
    const sf::Transform &transform) const
{
    target.draw(*this, transform);
}
