//
// Created by belle on 23/03/2026.
//

#pragma once
#include "Lumiere/Graphics/Lines.h"
#include "Lumiere/Renderer/IPass.h"
#include "slot_map/slot_map.h"

namespace lum::rdr
{
using DebugShapeHandle = dod::slot_map<std::vector<gfx::DebugVertex>>::key;
class Debug : public IPass
{
private:
    uint32_t m_width;
    uint32_t m_height;
    std::unique_ptr<gpu::Framebuffer> m_framebuffer {nullptr};

    // each inner vector represents one shape, the outer vector contains all these shapes.
    // allows for easier deletion of existing shapes without invalidating the whole structure
    dod::slot_map<std::vector<gfx::DebugVertex>> m_debugVertices {};
    gfx::Lines m_lineRenderer;
    bool m_dirty {false};
    bool m_shouldDrawGridNextFrame {true};

    // internal debug shapes storage
    void AddLine(std::vector<gfx::DebugVertex>& shape, const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    void EditLine(const DebugShapeHandle& shapeId, int index, const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    DebugShapeHandle RegisterBox(const glm::vec3& center, const glm::vec3& extent, const glm::vec3& color, const glm::mat4& transform = glm::mat4(1.0f));
    void EditBox(const DebugShapeHandle& index, const glm::vec3& center, const glm::vec3& extent, const glm::vec3& color, const glm::mat4& transform = glm::mat4(1.0f));
    DebugShapeHandle RegisterSphere(const glm::vec3& center, float radius, const glm::vec3& color, const glm::mat4& transform = glm::mat4(1.0f));
    void EditSphere(const DebugShapeHandle& index, const glm::vec3& center, float radius, const glm::vec3& color, const glm::mat4& transform = glm::mat4(1.0f));
public:
    static bool m_registered;
    #pragma region Constants
    constexpr static const char* GRID_SHADER_NAME = "DEBUG_GRID_SHADER";
    constexpr static const char* DEBUG_SHADER_NAME = "DEBUG_RENDERER_SHADER";
    #pragma endregion Constants

    Debug() = default;
    Debug(uint32_t width, uint32_t height);

    DebugShapeHandle DrawDebugBox(const DebugShapeHandle& index, const glm::vec3& center, const glm::vec3& extent, const glm::vec3& color, const glm::mat4& transform = glm::mat4(1.0f));
    DebugShapeHandle DrawDebugSphere(const DebugShapeHandle& index, const glm::vec3& center, float radius, const glm::vec3& color, const glm::mat4& transform = glm::mat4(1.0f));
    void RemoveShape(const DebugShapeHandle& key) { m_debugVertices.erase(key); };

    // packs all our vertices data for GPU
    std::vector<gfx::DebugVertex> PrepareData();

    void DrawGrid();

    void Init() override;
    void Render(const FrameData &frameData) override;
    void RenderUI() override;

    void Rebuild(uint32_t width, uint32_t height) override;

    void Serialize(YAML::Node passes) override;
    void Deserialize(YAML::Node pass) override;
};
} // lum::rdr