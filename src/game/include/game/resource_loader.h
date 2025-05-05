#pragma once


#include <cstdint>
#include <map>
#include <memory>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>


using TexturePtr = std::shared_ptr<const sf::Texture>;
using SoundBufferPtr = std::shared_ptr<const sf::SoundBuffer>;


class ResourceLoader final
{
public:
    enum TextureId : std::uint8_t
    {
        Splash,
        Player,
        Box,
        Crane,
        Background,
        Foreground,
        Digits,
        Frame,
        Hourglass
    };

    enum class SoundId : std::uint8_t
    {
        /// Звук, воспроизводимый в начале любого прыжка.
        Jump,
        /// Звук приземления.
        /// Воспроизводится только после завершения вертикального движения
        /// вниз, но не после завершения диагонального движения вниз и вбок.
        Land,
        /// Звук толкания ящика.
        Push,
        /// Звук сбивания ящиком. По-видимому, должен воспроизводиться только
        /// когда игрок сбивает ящик, находясь в наивысшей точке прыжка.
        Blow,
        /// Звук, воспроизводимый после заполнения нижнего ряда ящиков.
        Score,
        /// Звук окончания игры.
        GameOver
    };

public:
    /*!
     * Singleton instance accessor.
     * \return The reference to an instance of this class.
     */
    static ResourceLoader& instance();

    bool load();
    TexturePtr texture(const TextureId &id) const noexcept;
    SoundBufferPtr sound(const SoundId &id) const noexcept;

private:
    // Singleton part.
    ResourceLoader() = default;
    ~ResourceLoader() = default;
    ResourceLoader(const ResourceLoader&) = delete;
    ResourceLoader(ResourceLoader&&) = delete;
    ResourceLoader& operator=(ResourceLoader&&) = delete;
    ResourceLoader& operator=(const ResourceLoader&) = delete;

private:
    bool loadTexture(const TextureId id, const std::string &file);
    bool loadSound(const SoundId id, const std::string &file);

private:
    std::map<TextureId, std::shared_ptr<sf::Texture>> m_textures;
    std::map<SoundId, std::shared_ptr<sf::SoundBuffer>> m_sounds;
};
