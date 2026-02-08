//
// Created by Bellaedris on 03/02/2026.
//

#include "Bloom.h"

#include "ShadePBR.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/GPU/Shader.h"
#include "Lumiere/GPU/Texture.h"
#include "Lumiere/Renderer/RenderPipeline.h"

namespace lum::rdr
{
Bloom::Bloom(uint32_t width, uint32_t height)
    : m_width(width / 2)
    , m_height(height / 2)
{
    gpu::Texture::TextureDesc mipChainDesc
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_width),
        .height = static_cast<int>(m_height),
        .format = gpu::Texture::PixelFormat::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::LinearMipMapLinear,
        .magFilter = gpu::Texture::LinearMipMapLinear,
        .wrapMode = gpu::Texture::RepeatMirror
    };

    gpu::Texture::TextureDesc compositeDesc
    {
        .target = gpu::Texture::Target2D,
        .width = static_cast<int>(m_width * 2),
        .height = static_cast<int>(m_height * 2),
        .format = gpu::Texture::PixelFormat::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::Nearest,
        .magFilter = gpu::Texture::Nearest,
        .wrapMode = gpu::Texture::ClampToEdge
    };

    gpu::TexturePtr upsample = ResourcesManager::Instance()->CreateTexture(BLOOM_MASK_NAME, mipChainDesc);
    upsample->AllocateMipmapsStorage(6);

    ResourcesManager::Instance()->CreateTexture(BLOOM_NAME, compositeDesc);

    std::vector<gpu::Shader::ShaderSource> downsampleSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/bloom_downsample.comp"}
    };
    ResourcesManager::Instance()->CacheShader(BLOOM_DOWNSAMPLE_SHADER_NAME, downsampleSources);

    std::vector<gpu::Shader::ShaderSource> upsampleSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/bloom_upsample.comp"}
    };
    ResourcesManager::Instance()->CacheShader(BLOOM_UPSAMPLE_SHADER_NAME, upsampleSources);

    std::vector<gpu::Shader::ShaderSource> compositeSources =
    {
        {gpu::Shader::ShaderType::Compute, "shaders/bloom_composite.comp"}
    };
    ResourcesManager::Instance()->CacheShader(BLOOM_COMPOSITE_SHADER_NAME, compositeSources);
}

void Bloom::Render(const SceneDesc &scene)
{
    // downsample using the latest render
    gpu::ShaderPtr downsampleShader = ResourcesManager::Instance()->GetShader(BLOOM_DOWNSAMPLE_SHADER_NAME);
    downsampleShader->Bind();

    // first invocation uses the shaded image as input
    gpu::TexturePtr shaded = ResourcesManager::Instance()->GetTexture(ShadePBR::SHADE_PBR_NAME);
    shaded->Bind(0);

    gpu::TexturePtr mipChain = ResourcesManager::Instance()->GetTexture(BLOOM_MASK_NAME);
    mipChain->BindImage(1, 0, gpu::GLUtils::Access::Write);

    downsampleShader->UniformData("mipLevel", 1);
    downsampleShader->Dispatch(std::ceil(m_width / 16) + 1, std::ceil(m_height / 16) + 1, 1);
    downsampleShader->Wait();

    // subsequent invocations will read the above mip level
    float divider = 1.f;
    for (int i = 1; i < MIP_CHAIN_SIZE; i++)
    {
        divider *= .5f;
        mipChain->Bind(0);
        mipChain->BindImage(1, i, gpu::GLUtils::Access::Read);

        downsampleShader->UniformData("mipLevel", i);
        downsampleShader->Dispatch(std::ceil(m_width * divider / 16) + 1, std::ceil(m_height * divider / 16) + 1, 1);
        downsampleShader->Wait();
    }

    // upsample!
    gpu::ShaderPtr upsampleShader = ResourcesManager::Instance()->GetShader(BLOOM_UPSAMPLE_SHADER_NAME);
    upsampleShader->Bind();

    // we can even keep our divider from the previous loop
    divider *= 2.f;
    for (int i = MIP_CHAIN_SIZE - 2; i >= 0; i--)
    {
        mipChain->Bind(0);
        mipChain->BindImage(1, i, gpu::GLUtils::Access::ReadWrite);

        upsampleShader->UniformData("mipLevel", i);
        upsampleShader->Dispatch(std::ceil(m_width * divider / 16) + 1, std::ceil(m_height * divider / 16) + 1, 1);
        upsampleShader->Wait();

        divider *= 2.f;
    }

    // compose
    gpu::ShaderPtr compositeShader = ResourcesManager::Instance()->GetShader(BLOOM_COMPOSITE_SHADER_NAME);
    compositeShader->Bind();
    compositeShader->UniformData("intensity", m_intensity);

    shaded->Bind(0);
    compositeShader->UniformData("source", 0);
    mipChain->Bind(1);
    compositeShader->UniformData("bloomInput", 1);
    gpu::TexturePtr result = ResourcesManager::Instance()->GetTexture(BLOOM_NAME);
    result->BindImage(3, 0, gpu::GLUtils::Write);

    compositeShader->Dispatch(std::ceil(m_width * 2 / 16) + 1, std::ceil(m_height * 2 / 16) + 1, 1);
    compositeShader->Wait();
}

void Bloom::RenderUI()
{
    if (ImGui::TreeNode("Bloom"))
    {
        ImGui::SliderFloat("Intensity", &m_intensity, .0f, 1.f);
        ImGui::TreePop();
    }
}

void Bloom::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width / 2;
    m_height = height / 2;

    // once we assigned the texture storage with Texture::AllocateMipmapsStorage, the texture becomes immutable. We
    // cannot reallocate its size, we have to recreate a new texture.
    int iWidth = static_cast<int>(m_width);
    int iHeight = static_cast<int>(m_height);

    gpu::Texture::TextureDesc mipChainDesc
    {
        .target = gpu::Texture::Target2D,
        .width = iWidth,
        .height = iHeight,
        .format = gpu::Texture::PixelFormat::RGBA,
        .dataType = gpu::GLUtils::Float,
        .minFilter = gpu::Texture::LinearMipMapLinear,
        .magFilter = gpu::Texture::LinearMipMapLinear,
        .wrapMode = gpu::Texture::RepeatMirror
    };

    gpu::TexturePtr downsample = ResourcesManager::Instance()->CreateTexture(BLOOM_MASK_NAME, mipChainDesc);
    downsample->AllocateMipmapsStorage(6);

    gpu::TexturePtr composite = ResourcesManager::Instance()->GetTexture(BLOOM_NAME);
    composite->SetSize(static_cast<int>(width), static_cast<int>(height));
    composite->Reallocate();
}
}
