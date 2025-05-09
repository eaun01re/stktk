#include <game/objects/hourglass.h>

#include <game/consts.h>
#include <game/resource_loader.h>


Hourglass::Hourglass()
{
    init();
}


void Hourglass::init()
{
    ResourceLoader &resourceLoader = ResourceLoader::instance();
    setTexture(*resourceLoader.texture(ResourceLoader::TextureId::Hourglass));
    setColor(BACKGROUND_COLOR);
}
