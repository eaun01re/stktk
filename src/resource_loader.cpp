#include "resource_loader.h"

#include "log.h"


namespace
{

const std::string RESOURCES_DIR("resources/");

const std::string TEXTURE_BACKGROUND("background.png");
const std::string TEXTURE_BOX("box.png");
const std::string TEXTURE_CRANE("crane.png");
const std::string TEXTURE_FOREGROUND("foreground.png");
const std::string TEXTURE_PLAYER("player.png");

}


ResourceLoader& ResourceLoader::instance()
{
    static ResourceLoader singleton;
    return singleton;
}


bool ResourceLoader::load()
{
    const bool result =
        loadTexture(TextureId::Box, TEXTURE_BOX) &&
        loadTexture(TextureId::Player, TEXTURE_PLAYER) &&
        loadTexture(TextureId::Background, TEXTURE_BACKGROUND) &&
        loadTexture(TextureId::Foreground, TEXTURE_FOREGROUND) &&
        loadTexture(TextureId::Crane, TEXTURE_CRANE);
    return result;
}


TexturePtr ResourceLoader::texture(const TextureId &id) const noexcept
{
    const auto it = m_textures.find(id);
    if (it == m_textures.cend())
    {
        return nullptr;
    }

    return TexturePtr(it->second);
}


bool ResourceLoader::loadTexture(
    const TextureId id,
    const std::string &file)
{
    std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const bool ok = texture->loadFromFile(RESOURCES_DIR + file);
    if (!ok)
    {
        LOG_ERROR("Could not load resource \"" << file << "\".");
        return false;
    }
    m_textures[id] = texture;
    return ok;
}
