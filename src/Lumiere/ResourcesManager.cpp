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

gpu::TexturePtr ResourcesManager::GetTexture(size_t hash)
{
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
} // lum