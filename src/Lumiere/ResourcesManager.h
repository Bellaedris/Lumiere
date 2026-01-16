//
// Created by Bellaedris on 16/01/2026.
//

#pragma once
#include <map>

#include "GPU/Texture.h"
#include "GPU/Shader.h"

namespace lum
{
/**
 * \brief Owns and caches all the "cumbersome" resources of the engine: Textures, Shaders, Meshes. In the future, this
 * class might be split with a "Manager" dedicated for each resource that needs more complexity. We keep a simple model for now.
 */
class ResourcesManager
{
private:
    ResourcesManager() = default;
    #pragma region Members
    static ResourcesManager* m_instance;

    std::map<size_t, gpu::TexturePtr> m_textureCache;
    std::map<std::string, gpu::ShaderPtr> m_shaderCache;
    #pragma endregion Members

public:
    // we can't copy the resource manager singleton
    ResourcesManager(const ResourcesManager&) = delete;
    ResourcesManager& operator=(const ResourcesManager&) = delete;

    ~ResourcesManager();

    static ResourcesManager* Instance();

    #pragma region Texture Cache
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
    #pragma endregion Texture Cache

    #pragma region Shader cache
    // In the future, there should be a dedicated shader manager to handle compilation to SPIR-V, early caching of already
    // built shaders, permutations... But since it does very few things for now, it will stay in a general "resources manager"

    // names of default shaders, created by the engine at initialisation. In the future, we will want
    // a config file that holds all shader programs to load
    constexpr static const char* PBR_LIT_SHADER_KEY = "PBR_LIT";
    constexpr static const char* NPR_LIT_SHADER_KEY = "NPR_LIT";

    gpu::ShaderPtr GetShader(const std::string &name);
    gpu::ShaderPtr CacheShader(const std::string& name, const std::vector<gpu::Shader::ShaderSource> &shaderSources);
    void ShaderHotReload();
    #pragma endregion Shader cache
};
} // lum