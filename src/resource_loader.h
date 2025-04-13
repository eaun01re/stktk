#pragma once


#include <cstdint>
#include <map>
#include <memory>

#include <SFML/Graphics.hpp>


using TexturePtr = std::shared_ptr<const sf::Texture>;


class ResourceLoader final
{
public:
    enum class TextureId : std::uint8_t
    {
        Player,
        Box,
        Crane,
        Background,
        Foreground,
        Digits,
        Frame,
        Hourglass
    };

public:
    /*!
     * Singleton instance accessor.
     * \return The reference to an instance of this class.
     */
    static ResourceLoader& instance();

    bool load();
    TexturePtr texture(const TextureId &id) const noexcept;

private:
    // Singleton part.
    ResourceLoader() = default;
    ~ResourceLoader() = default;
    ResourceLoader(const ResourceLoader&) = delete;
    ResourceLoader(ResourceLoader&&) = delete;
    ResourceLoader& operator=(ResourceLoader&&) = delete;
    ResourceLoader& operator=(const ResourceLoader&) = delete;

private:
    bool loadTexture(
        const TextureId id,
        const std::string &file);

private:
    std::map<TextureId, std::shared_ptr<sf::Texture>> m_textures;
};
