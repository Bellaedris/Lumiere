//
// Created by belle on 30/04/2026.
//

#include "MaterialOverrideBlock.h"

#include <ranges>

namespace lum::gfx
{
void MaterialOverrideBlock::Bind(const gpu::ShaderPtr &shader) const
{
    for (const auto& [uniformLocation, value] : m_floatBlocks)
        shader->UniformData(uniformLocation, value);
    for (const auto& [uniformLocation, value] : m_vectorBlocks)
        shader->UniformData(uniformLocation, value);
}

void MaterialOverrideBlock::Clear()
{
    m_floatBlocks.clear();
    m_vectorBlocks.clear();
}

void MaterialOverrideBlock::SetFloat(const std::string &location, float value)
{
    m_floatBlocks[location] = value;
}

void MaterialOverrideBlock::SetVector(const std::string &location, const glm::vec3 &value)
{
    m_vectorBlocks[location] = value;
}
} // lum::gfx