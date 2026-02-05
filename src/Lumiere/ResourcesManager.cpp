//
// Created by Bellaedris on 16/01/2026.
//

#include "ResourcesManager.h"

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

    gpu::TexturePtr white = std::make_shared<gpu::Texture>(gpu::Texture::Target2D);
    float dataWhite[4] = {1.f, 1.f, 1.f, 1.f};
    white->Write(dataWhite, gpu::Texture::RGBA, gpu::GLUtils::Float);
    m_textureCache.emplace(std::hash<std::string>{}(DEFAULT_TEXTURE_WHITE_NAME), white);

    gpu::TexturePtr black = std::make_shared<gpu::Texture>(gpu::Texture::Target2D);
    float dataBlack[4] = {0.f, 0.f, 0.f, 0.f};
    white->Write(dataBlack, gpu::Texture::RGBA, gpu::GLUtils::Float);
    m_textureCache.emplace(std::hash<std::string>{}(DEFAULT_TEXTURE_BLACK_NAME), black);
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
}
} // lum