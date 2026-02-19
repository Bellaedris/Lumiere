//
// Created by Bellaedris on 20/01/2026.
//

#pragma once
#include "SceneDesc.h"
#include "Lumiere/GPU/Framebuffer.h"
#include <imgui/imgui.h>

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>
#include "Lumiere/ProfilerGPU.h"
#include "Passes/PassFactory.h"

namespace lum::rdr
{

struct FrameData
{
    const std::shared_ptr<SceneDesc>& scene;
    int frameIndex;
    const std::shared_ptr<ProfilerGPU>& profilerGPU;
};

/**
 * \brief A pass is a framebuffer that will be drawn by reading a scene and its parameters
 */
class IPass
{
#define REGISTER_TO_PASS_FACTORY(type, name) bool type::m_registered = PassFactory::Register(name, []() { return std::make_unique<type>(); } );
public:
    virtual ~IPass() = default;

    /**
     * \brief Initializes the pass resources
     */
    virtual void Init() = 0;

    /**
     * \brief Draws things
     * \param frameData all datas relevant to draw this frame
     */
    virtual void Render(const FrameData &frameData) = 0;

    /**
     * \brief Draws a debug UI holding infos about the pass
     */
    virtual void RenderUI() = 0;

    /**
     * \brief Rebuild the pass to account for a size update. Called upon receiving a size change event
     * \param width new viewport width
     * \param height new viewport height
     */
    virtual void Rebuild(uint32_t width, uint32_t height) = 0;

    /**
     * \brief Write the pass infos to a YAML stream
     * \param passes a node containing an array describing all passes in a render pipeline. A pass should add a new node to
     * this array, using passes.push_back(node_for_this_pass)
     */
    virtual void Serialize(YAML::Node passes) = 0;

    /**
     * \brief Reads a pass info to reconstruct itself from yaml data
     * \param pass a node containing the same datas that were serialized in yaml
     */
    virtual void Deserialize(YAML::Node pass) = 0;
};
} // lum::rdr