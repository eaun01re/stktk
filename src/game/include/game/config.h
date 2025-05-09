#pragma once


#include <cstdint>


class Config final
{
public:
    /*!
     * Singleton instance accessor.
     * \return The reference to an instance of this class.
     */
    static Config& instance();

private:
    // Singleton part.
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;
    Config& operator=(const Config&) = delete;

public:
    std::uint8_t cranesQuantity{ 1 };
    bool sound{ true };
};
