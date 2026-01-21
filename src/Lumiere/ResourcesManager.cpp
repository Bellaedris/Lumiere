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
        m_instance = new ResourcesManager();

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

    gpu::TexturePtr cached = std::make_shared<gpu::Texture>(desc.target);
    cached->SetSize(desc.width, desc.height);
    cached->SetMagFilter(desc.magFilter);
    cached->SetMinFilter(desc.minFilter);
    cached->SetWrapMode(desc.wrapMode);
    cached->Allocate(desc.format, desc.dataType);
    m_textureCache.emplace(hash, cached);

    return cached;
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
} // lum