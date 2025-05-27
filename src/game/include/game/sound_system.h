#pragma once


#include <SFML/Audio.hpp>

#include "resource_loader.h"


class SoundSystem final
{
public:
    /*!
     * Singleton instance accessor.
     * \return The reference to an instance of this class.
     */
    static SoundSystem& instance();
    void setEnabled(bool value);

    void playSound(ResourceLoader::SoundId id);

private:
    // Singleton part.
    SoundSystem();
    ~SoundSystem() = default;
    SoundSystem(const SoundSystem&) = delete;
    SoundSystem(SoundSystem&&) = delete;
    SoundSystem& operator=(SoundSystem&&) = delete;
    SoundSystem& operator=(const SoundSystem&) = delete;

private:
    void setup();

private:
    sf::Sound m_sound;
    bool m_enabled{ false };
    ResourceLoader &m_resourceLoader;
};
