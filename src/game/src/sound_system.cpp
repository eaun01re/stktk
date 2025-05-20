#include <game/sound_system.h>

#include <game/config.h>


SoundSystem& SoundSystem::instance()
{
    static SoundSystem singleton;
    return singleton;
}


void SoundSystem::setEnabled(bool value)
{
    m_enabled = value;
}


void SoundSystem::playSound(ResourceLoader::SoundId id)
{
    if (!m_enabled)
    {
        return;
    }

    // TODO: Сделать параллельное воспроизведение звуков?
    m_sound.setBuffer(*m_resourceLoader.sound(id));
    m_sound.play();
}


SoundSystem::SoundSystem()
    : m_resourceLoader(ResourceLoader::instance())
{
    setup();
}


void SoundSystem::setup()
{
    setEnabled(Config::instance().sound);
}
