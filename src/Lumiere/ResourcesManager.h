//
// Created by Bellaedris on 16/01/2026.
//

#pragma once
#include <map>

#include "GPU/Texture.h"
#include "GPU/Shader.h"
#include "Graphics/Mesh.h"

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

    /** \brief Texture cache is indexed by a hash of the texture path */
    std::map<size_t, gpu::TexturePtr> m_textureCache;
    /** \brief Shader cache is indexed by the name of each shader */
    std::map<std::string, gpu::ShaderPtr> m_shaderCache;
    /** \brief Mesh cache is indexed by a hash of the mesh's path */
    std::map<size_t, gfx::MeshPtr> m_meshCache;
    #pragma endregion Members

public:
    #pragma region Default Names
    constexpr static const char* DEFAULT_PLANE_NAME = "Lumiere_default_plane";
    constexpr static const char* DEFAULT_TEXTURE_WHITE_NAME = "Lumiere_default_texture_white";
    constexpr static const char* DEFAULT_TEXTURE_BLACK_NAME = "Lumiere_default_texture_black";
    #pragma endregion Default Names

    // we can't copy the resource manager singleton
    ResourcesManager(const ResourcesManager&) = delete;
    ResourcesManager& operator=(const ResourcesManager&) = delete;

    ~ResourcesManager();

    static ResourcesManager* Instance();

    #pragma region Texture Cache
    /**
     * \brief Try to access a texture, if it exists
     * \param name The path to this texture hashed through std::hash<std::string>
     * \return The texture if it was cached, nullptr otherwise
     */
    gpu::TexturePtr GetTexture(std::string name);

    /**
     * \brief Constructs a texture in place and caches it.
     * \param target Type of texture to construct. \see gpu::Texture::TextureTarget
     * \param path Path to the texture
     * \param generateMipmaps True to generate a mip chain
     * \return A handle to the newly cached texture
     */
    gpu::TexturePtr CacheTexture(gpu::Texture::TextureTarget target, const std::string &path, bool generateMipmaps);

    /**
     * \brief Create an empty texture in place and caches it. Used to create the textures we render to, for instance.
     * \param name Name of the texture used to identify. For instance, "GBUFFER_ALBEDO"
     * \param target What kind of texture we are creating
     * \param format Format of the created texture
     * \param dataType Type of data in the created texture
     * \return A handle to the newly cached texture
     */
    gpu::TexturePtr CreateTexture(const std::string& name, const gpu::Texture::TextureDesc &);

    /**
     * \brief Create default all white and all black default textures as fallback when a material lacks a specific texture
     */
    void GenerateDefaultTextures();
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

    #pragma region Mesh Cache
    /**
     * \brief Try to access a texture, if it exists
     * \param path The path to this texture
     * \return The texture if it was cached, nullptr otherwise
     */
    gfx::MeshPtr GetMesh(const std::string &path);

    /**
     * \brief Constructs a Mesh in place, caches it, and returns its handle
     * \param path Path to this mesh
     * \param meshData an array of all its submeshes
     * \return A handle to the newly cached Mesh
     */
    gfx::MeshPtr CacheMesh(const std::string& path, std::vector<gfx::SubMesh> &meshData);

    void GenerateDefaultMeshes();
    #pragma endregion Mesh Cache
};
} // lum