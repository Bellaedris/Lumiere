//
// Created by Bellaedris on 16/01/2026.
//

#pragma once
#include <map>

#include "GPU/Texture.h"
#include "GPU/Shader.h"
#include "Graphics/Mesh.h"
#include "stduuid/uuid.h"

namespace lum
{
#pragma region Structs
struct MaterialRegistryEntry
{
    bool loaded;
    std::string path;
};
#pragma endregion // Structs

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
    /** \brief Material cache is indexed by a hash of the material's path */
    std::map<std::string, gfx::MaterialPtr> m_materialCache;
    /**
     * \brief Lists all known materials and wether or not they are registered.
     * This way we can have a list of available materials without loading them in GPU memory right away
     */
    std::map<std::string, MaterialRegistryEntry> m_materialRegistry;
    #pragma endregion Members

public:
    #pragma region Constants
    constexpr static const char* DEFAULT_PLANE_NAME = "Lumiere_default_plane";
    constexpr static const char* DEFAULT_SPHERE_NAME = "Lumiere_default_sphere";
    constexpr static const char* DEFAULT_TEXTURE_WHITE_NAME = "Lumiere_default_texture_white";
    constexpr static const char* DEFAULT_TEXTURE_BLACK_NAME = "Lumiere_default_texture_black";
    constexpr static const char* DEFAULT_MATERIAL_PBR_LIT = "Lumiere_default_PBR_lit";

    constexpr static const char* MATERIAL_SAVE_PATH = "Assets/Materials/";
    #pragma endregion Constants

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
     * Calling this method with a name that is already cached will delete the original texture and create a new one.
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

    void                                             GenerateDefaultMeshes();
    /**
     * \brief Gathers all mesh names so they can be displayed, for instance in an asset-manager like widget of the editor
     * \return A vector with all the cached meshes' path as well as a more readable identifier
     */
    std::vector<std::pair<std::string, std::string>> MeshNames();
    #pragma endregion Mesh Cache

    #pragma region Material cache
    /**
     * \brief Try to access a material, if it exists
     * \param name name of this material - usually extracted from a loaded mesh, or user-specified
     * \return optional material or nullopt
     */
    std::optional<gfx::MaterialPtr> GetMaterial(const std::string& name);

    /**
     * \brief Caches a Material, serialize it and return its uuid
     * \param material material to cache
     * \return A handle to the cached Material
     */
    void CacheMaterial(gfx::MaterialPtr& material);

    /**
     * \brief Adds a Material to the registry. If the material was registered already, does nothing
     * \param name name of the material. Should match with the actual cache entry
     * \param loaded is the material already loaded in cache?
     */
    void RegisterMaterial(const std::string& name, bool loaded);

    /**
     * \brief Loads a material file and caches it. Updates the registry status if the loading is successful
     * \param name name of the material to load
     * \return The cached material, or nullopt if the material couldn't be loaded
     */
    std::optional<gfx::MaterialPtr> LoadMaterialFromRegistry(const std::string& name);

    /**
     * \brief Try to get an element of the material register
     * \param name name of the material
     * \return nullopt if the material is not registered, a boolean representing the cache loading status of the material
     * if registered (true if the mat is loaded in cache, false if not)
     */
    std::optional<MaterialRegistryEntry> GetRegistryEntry(const std::string& name);

    /**
     * \brief Initializes the material registry by reading every material file inside the Assets/Materials folder. Does not
     * load the resources yet, but it allows the ResourcesManager to be aware of what resources are available and where they should be loaded
     */
    void BuildMaterialRegistry();

    /**
     * \brief Gives the string handle of all available materials. Useful for the editor
     * \return An array with all material handles
     */
    std::vector<std::string> MaterialNames() const;

    void GenerateDefaultMaterials();
    #pragma endregion // Material cache
};
} // lum