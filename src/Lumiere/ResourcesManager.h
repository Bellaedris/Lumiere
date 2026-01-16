//
// Created by Bellaedris on 16/01/2026.
//

#pragma once
#include <map>

#include "GPU/Texture.h"

namespace lum
{
class ResourcesManager
{
private:
    ResourcesManager() = default;
    #pragma region Members
    static ResourcesManager* m_instance;

    std::map<size_t, gpu::TexturePtr> m_textureCache;
    #pragma endregion Members

public:
    // we can't copy the resource manager singleton
    ResourcesManager(const ResourcesManager&) = delete;
    ResourcesManager& operator=(const ResourcesManager&) = delete;

    ~ResourcesManager();

    static ResourcesManager* Instance();

    #pragma region Texture Cache methods
    /**
     * \brief Try to access a texture, if it exists
     * \param hash The path to this texture hashed through std::hash<std::string>
     * \return The texture if it was cached, nullptr otherwise
     */
    gpu::TexturePtr GetTexture(size_t hash);

    /**
     * \brief Constructs a texture in place and caches it.
     * \param target Type of texture to construct. \see gpu::Texture::TextureTarget
     * \param path Path to the texture
     * \param generateMipmaps True to generate a mip chain
     * \return A handle to the newly cached texture
     */
    gpu::TexturePtr CacheTexture(gpu::Texture::TextureTarget target, const std::string &path, bool generateMipmaps);
    #pragma endregion Texture Cache methods
};
} // lum