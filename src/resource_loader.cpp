#include "resource_loader.h"

#include <filesystem>

#include "log.h"


namespace
{

const std::filesystem::path RESOURCES_DIR("resources");
const std::filesystem::path PATH_GRAPHICS = RESOURCES_DIR / "graphics";
const std::filesystem::path PATH_AUDIO = RESOURCES_DIR / "audio";

const std::string TEXTURE_SPLASH("splash.png");
const std::string TEXTURE_BACKGROUND("background.png");
const std::string TEXTURE_BOX("box.png");
const std::string TEXTURE_CRANE("crane.png");
const std::string TEXTURE_FOREGROUND("foreground.png");
const std::string TEXTURE_PLAYER("player.png");
const std::string TEXTURE_DIGITS("digits.png");
const std::string TEXTURE_FRAME("frame.png");
const std::string TEXTURE_HOURGLASS("hourglass.png");

const std::string SOUND_JUMP("jump.ogg");
const std::string SOUND_LAND("land.ogg");
const std::string SOUND_PUSH("push.ogg");
const std::string SOUND_BLOW("blow.ogg");
const std::string SOUND_SCORE("score.ogg");
const std::string SOUND_GAME_OVER("game_over.ogg");

}


ResourceLoader& ResourceLoader::instance()
{
    static ResourceLoader singleton;
    return singleton;
}


bool ResourceLoader::load()
{
    const bool result =
        loadTexture(TextureId::Splash, TEXTURE_SPLASH) &&
        loadTexture(TextureId::Box, TEXTURE_BOX) &&
        loadTexture(TextureId::Player, TEXTURE_PLAYER) &&
        loadTexture(TextureId::Background, TEXTURE_BACKGROUND) &&
        loadTexture(TextureId::Foreground, TEXTURE_FOREGROUND) &&
        loadTexture(TextureId::Crane, TEXTURE_CRANE) &&
        loadTexture(TextureId::Digits, TEXTURE_DIGITS) &&
        loadTexture(TextureId::Frame, TEXTURE_FRAME) &&
        loadTexture(TextureId::Hourglass, TEXTURE_HOURGLASS)
        &&
        loadSound(SoundId::Jump, SOUND_JUMP) &&
        loadSound(SoundId::Land, SOUND_LAND) &&
        loadSound(SoundId::Push, SOUND_PUSH) &&
        loadSound(SoundId::Blow, SOUND_BLOW) &&
        loadSound(SoundId::Score, SOUND_SCORE) &&
        loadSound(SoundId::GameOver, SOUND_GAME_OVER);
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


SoundBufferPtr ResourceLoader::sound(const SoundId &id) const noexcept
{
    const auto it = m_sounds.find(id);
    if (it == m_sounds.cend())
    {
        return nullptr;
    }

    return SoundBufferPtr(it->second);
}


bool ResourceLoader::loadTexture(
    const TextureId id,
    const std::string &file)
{
    std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const bool ok = texture->loadFromFile((PATH_GRAPHICS / file).string());
    if (!ok)
    {
        LOG_ERROR("Could not load resource \"" << file << "\".");
        return false;
    }
    m_textures[id] = texture;
    return ok;
}


bool ResourceLoader::loadSound(
    const SoundId id,
    const std::string &file)
{
    std::shared_ptr<sf::SoundBuffer> soundBuffer = std::make_shared<sf::SoundBuffer>();
    const bool ok = soundBuffer->loadFromFile((PATH_AUDIO / file).string());
    if (!ok)
    {
        LOG_ERROR("Could not load resource \"" << file << "\".");
        return false;
    }
    m_sounds[id] = soundBuffer;
    return ok;
}
