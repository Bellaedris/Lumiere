//
// Created by Bellaedris on 14/01/2026.
//

#pragma once

#include <memory>
#include <Lumiere/GPU/Shader.h>

#include "MaterialFactory.h"
#include "yaml-cpp/node/node.h"

namespace lum::gfx
{
/**
 * \brief Exposes the basics of a Material: it can send datas to a shader. The datas are defined by the implementations
 * For instance, a MaterialPBR will have textures containing the albedo, normal, etc... That the Bind method will send
 * to the shader associated with the material.
 */
class IMaterial
{
protected:
    std::string m_name;
    #define REGISTER_TO_MATERIAL_FACTORY(type, name) bool type::m_registered = MaterialFactory::Register(name, [](const std::string& n) { return std::make_shared<type>(n); } );
public:
    constexpr static const char* MATERIAL_FILE_FORMAT = ".lumat";

    IMaterial(const std::string& name) : m_name(name) {}
    virtual ~IMaterial() = default;
    /**
     * \brief Activates the shader associated with this material and send all the required datas referenced
     */
    virtual void Bind(const gpu::ShaderPtr& shader) = 0;

    virtual void DrawEditor() = 0;

    std::string Name() { return m_name; }

    virtual void Serialize(const std::string& path) const = 0;
    virtual void Deserialize(YAML::Node& node) = 0;
};
using MaterialPtr = std::shared_ptr<IMaterial>;
}
