//
// Created by Bellaedris on 16/01/2026.
//

#include "ResourcesManager.h"

#include <ranges>

#include "Graphics/MaterialPBR.h"

namespace lum
{
ResourcesManager* ResourcesManager::m_instance = nullptr;

ResourcesManager::~ResourcesManager()
{
    m_instance = nullptr;
}

ResourcesManager* ResourcesManager::Instance()
{
    if (m_instance == nullptr)
    {
        m_instance = new ResourcesManager();
        m_instance->GenerateDefaultMeshes();
        m_instance->GenerateDefaultTextures();
        m_instance->GenerateDefaultMaterials();
        m_instance->BuildMaterialRegistry();
    }

    return m_instance;
}

gpu::TexturePtr ResourcesManager::GetTexture(std::string path)
{
    size_t hash = std::hash<std::string>{}(path);

    if (const auto it = m_textureCache.find(hash); it != m_textureCache.end())
        return it->second;

    return nullptr;
}

gpu::TexturePtr ResourcesManager::CacheTexture(gpu::Texture::TextureTarget target, const std::string &path, bool generateMipmaps)
{
    size_t hash = std::hash<std::string>{}(path);

    gpu::TexturePtr cached = std::make_shared<gpu::Texture>(target, path.c_str(), generateMipmaps);
    m_textureCache.emplace(hash, cached);

    return cached;
}

gpu::TexturePtr ResourcesManager::CreateTexture
(
    const std::string &              name,
    const gpu::Texture::TextureDesc &desc
)
{
    size_t hash = std::hash<std::string>{}(name);
    if (m_textureCache.contains(hash))
    {
        m_textureCache.erase(hash);
    }

    gpu::TexturePtr cached = std::make_shared<gpu::Texture>(desc.target);
    cached->SetSize(desc.width, desc.height);
    cached->SetMagFilter(desc.magFilter);
    cached->SetMinFilter(desc.minFilter);
    cached->SetWrapMode(desc.wrapMode);
    cached->Allocate(desc.format, desc.dataType);
    m_textureCache.emplace(hash, cached);

    return cached;
}

void ResourcesManager::GenerateDefaultTextures()
{
    gpu::Texture::TextureDesc desc =
    {
        .target = gpu::Texture::TextureTarget::Target2D,
        .width = 1,
        .height = 1,
        .format = gpu::Texture::PixelFormat::RGBA,
        .dataType = gpu::GLUtils::DataType::Float,
        .minFilter = gpu::Texture::Filtering::Nearest,
        .magFilter = gpu::Texture::Filtering::Nearest,
        .wrapMode = gpu::Texture::WrapMode::ClampToEdge
    };

    gpu::TexturePtr white = CreateTexture(DEFAULT_TEXTURE_WHITE_NAME, desc);
    float dataWhite[4] = {1.f, 1.f, 1.f, 1.f};
    white->Write(dataWhite, gpu::Texture::RGBA, gpu::GLUtils::Float);

    gpu::TexturePtr black = CreateTexture(DEFAULT_TEXTURE_BLACK_NAME, desc);
    float dataBlack[4] = {0.f, 0.f, 0.f, 0.f};
    black->Write(dataBlack, gpu::Texture::RGBA, gpu::GLUtils::Float);
}

void ResourcesManager::GenerateDefaultMaterials()
{
    gfx::MaterialPtr defaultLit = std::make_unique<gfx::MaterialPBR>(DEFAULT_MATERIAL_PBR_LIT);
    CacheMaterial(defaultLit);
}

gpu::ShaderPtr ResourcesManager::GetShader(const std::string &name)
{
    if (const auto it = m_shaderCache.find(name); it != m_shaderCache.end())
        return it->second;

    return nullptr;
}

gpu::ShaderPtr ResourcesManager::CacheShader(const std::string& name, const std::vector<gpu::Shader::ShaderSource> &shaderSources)
{
    gpu::ShaderPtr shader = std::make_shared<gpu::Shader>();
    for (const auto& shaderSource : shaderSources)
        shader->AddShaderFromFile(shaderSource);

    m_shaderCache.emplace(name, shader);

    return shader;
}

void ResourcesManager::ShaderHotReload()
{
    for (auto& cacheElement : m_shaderCache)
        cacheElement.second->Reload();
}

gfx::MeshPtr ResourcesManager::GetMesh(const std::string &path)
{
    size_t hash = std::hash<std::string>{}(path);

    if (const auto it = m_meshCache.find(hash); it != m_meshCache.end())
        return it->second;

    return nullptr;
}

gfx::MeshPtr ResourcesManager::CacheMesh(const std::string& path, std::vector<gfx::SubMesh> &meshData)
{
    size_t hash = std::hash<std::string>{}(path);

    gfx::MeshPtr cached = std::make_shared<gfx::Mesh>(path, meshData);
    m_meshCache.emplace(hash, cached);

    return cached;
}

void ResourcesManager::GenerateDefaultMeshes()
{
    std::vector<gfx::SubMesh> plane = gfx::Mesh::GeneratePlane(1.f);
    CacheMesh(DEFAULT_PLANE_NAME, plane);

    std::vector<gfx::SubMesh> sphere = gfx::Mesh::GenerateSphere(1.f);
    CacheMesh(DEFAULT_SPHERE_NAME, sphere);
}

std::vector<std::pair<std::string, std::string>> ResourcesManager::MeshNames()
{
    // TODO change it to be cached and only get rebuilt when we add/remove meshes
    std::vector<std::pair<std::string, std::string>> names;
    names.reserve(m_meshCache.size());
    for (const auto& m : m_meshCache)
        names.emplace_back(m.second->Path(), m.second->Name());
    return names;
}

std::optional<gfx::MaterialPtr> ResourcesManager::GetMaterial(const std::string &name)
{
    if (auto it = m_materialCache.find(name); it != m_materialCache.end())
    {
        return { it->second };
    }

    return {};
}

void ResourcesManager::CacheMaterial(gfx::MaterialPtr &material)
{
    m_materialCache.emplace(material->Name(), material);
    material->Serialize(MATERIAL_SAVE_PATH);
    std::cerr << "loaded material " << material->Name() << "\n";
}

void ResourcesManager::RegisterMaterial(const std::string &name, bool loaded)
{
    if (m_materialRegistry.contains(name) == false)
        m_materialRegistry.emplace(name, loaded);
}

std::optional<gfx::MaterialPtr> ResourcesManager::LoadMaterialFromRegistry(const std::string &name)
{
    if (m_materialRegistry.contains(name) == false)
        return {};

    MaterialRegistryEntry entry = m_materialRegistry[name];
    // skip the loading if the entry is loaded already, although it shouldn't happen
    if (entry.loaded == true)
        return m_materialCache[name];

    YAML::Node root;
    try
    {
        root = YAML::LoadFile(entry.path);
    }
    catch (YAML::BadFile& e)
    {
        std::cerr << "Incompatible material file.\n";
        return {};
    }

    gfx::MaterialPtr mat = gfx::MaterialFactory::Create(root["type"].as<std::string>(), name);
    mat->Deserialize(root);
    m_materialCache.emplace(name, mat);

    entry.loaded = true;

    return {mat};
}

std::optional<MaterialRegistryEntry> ResourcesManager::GetRegistryEntry(const std::string &name)
{
    if (m_materialRegistry.contains(name))
        return {m_materialRegistry[name]};

    return {};
}

void ResourcesManager::BuildMaterialRegistry()
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(MATERIAL_SAVE_PATH))
    {
        if (entry.is_regular_file() == false)
            continue;

        std::string name = entry.path().filename().stem().c_str();
        m_materialRegistry.emplace(name, MaterialRegistryEntry(false, entry.path()));
    }
}

std::vector<std::string> ResourcesManager::MaterialNames() const
{
    std::vector<std::string> names;
    names.reserve(m_materialRegistry.size());
    for (const auto &key: m_materialRegistry | std::views::keys)
        names.push_back(key);

    return names;
}
} // lum