//
// Created by belle on 30/04/2026.
//

#pragma once
#include <string>
#include <unordered_map>

#include "Lumiere/GPU/Shader.h"

namespace lum::gfx
{
class MaterialOverrideBlock
{
private:
    std::unordered_map<std::string, int> m_intBlocks;
    std::unordered_map<std::string, float> m_floatBlocks;
    std::unordered_map<std::string, glm::vec3> m_vectorBlocks;

public:
    /**
     * \brief Binds all overrides to a shader. The shader must be bound beforehand
     * \param shader A shader from a renderPass
     */
    void Bind(const gpu::ShaderPtr& shader) const;

    /**
     * \brief Clears all overrides
     */
    void Clear();

    /**
     * \brief assign a float value to a shader uniform location
     * \param location Name of the shader uniform to override
     * \param value value to assign to the shader uniform
     */
    void SetFloat(const std::string& location, float value);

    /**
     * \brief assign a vec3 value to a shader uniform location
     * \param location Name of the shader uniform to override
     * \param value value to assign to the shader uniform
     */
    void SetVector(const std::string& location, const glm::vec3& value);
};
} // lum::gfx