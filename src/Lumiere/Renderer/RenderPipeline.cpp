//
// Created by Bellaedris on 20/01/2026.
//

#include "RenderPipeline.h"

#include <fstream>
#include <memory>

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Events/RenderEvents.h"
#include "Passes/PassFactory.h"

namespace lum::rdr {
RenderPipeline::RenderPipeline(const std::string& name)
    : m_name(name)
{

}

void RenderPipeline::Render(const FrameData &frameData) const
{
    for (const auto& pass : m_passes)
        pass->Render(frameData);
}

void RenderPipeline::RenderUI()
{
    for (const std::shared_ptr<IPass>& pass : m_passes)
        pass->RenderUI();
}

void RenderPipeline::AddPass(const std::shared_ptr<IPass>& pass)
{
    m_passes.push_back(pass);
}

void RenderPipeline::Serialize() const
{
    YAML::Node root;
    root["version"] = SERIALIZER_VERSION;
    root["name"] = m_name;
    YAML::Node passesNode = root["passes"];

    for (const auto& pass : m_passes)
    {
        pass->Serialize(passesNode);
    }

    std::ofstream out("config/" + m_name + ".yaml");
    out << root;
}

void RenderPipeline::Deserialize(const YAML::Node &pipeline)
{
    if (pipeline["version"].as<int>() != SERIALIZER_VERSION)
    {
        std::cerr << "Serializer version mismatch. This pipeline uses deprecated serialization standards.\n";
        return;
    }

    // parse all passes and create the appropriate IPass implementation
    for (const auto& pass : pipeline["passes"])
    {
        std::unique_ptr<IPass> constructed = PassFactory::Create(pass["name"].as<std::string>());
        if (constructed != nullptr)
        {
            std::cerr << "Deserialized pass " << pass["name"].as<std::string>() << "\n";
            constructed->Deserialize(static_cast<YAML::Node>(pass));
            m_passes.push_back(std::move(constructed));
        }
    }
}

void RenderPipeline::Rebuild(const glm::vec2 &newSize)
{
    for (const auto& pass : m_passes)
        pass->Rebuild(static_cast<uint32_t>(newSize.x), static_cast<uint32_t>(newSize.y));
}

void RenderPipeline::OnEvent(const std::shared_ptr<evt::IEvent> &e)
{
    if (e->Type() == evt::FramebufferResized)
    {
        auto event = std::dynamic_pointer_cast<evt::FramebufferResizedEvent>(e);
        Rebuild(event->m_size);
    }
}
} // lum::rdr