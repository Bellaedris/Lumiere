//
// Created by Bellaedris on 14/01/2026.
//

#pragma once

#include <memory>
#include <Lumiere/GPU/Shader.h>

namespace lum::gfx
{
/**
 * \brief Exposes the basics of a Material: it is a Shader, with extra things in it that will be defined by subclasses.
 * For instance, a MaterialPBR will have textures containing the albedo, normal, etc... That the Bind method will send
 * to the shader associated with the material.
 */
class IMaterial
{
protected:
    gpu::ShaderPtr m_shader;
public:
    explicit IMaterial(const gpu::ShaderPtr& shader) : m_shader(shader) {};
    /**
     * \brief Activates the shader associated with this material and send all the required datas referenced
     */
    virtual void Bind() = 0;
};
using MaterialPtr = std::shared_ptr<IMaterial>;
}