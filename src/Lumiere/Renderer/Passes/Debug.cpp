//
// Created by belle on 23/03/2026.
//

#include "Debug.h"

#include "Lumiere/RendererManager.h"
#include "Lumiere/ResourcesManager.h"
#include "Lumiere/Components/Collider.h"
#include "Lumiere/GPU/Buffer.h"

namespace lum::rdr
{
// note that this pass has a special status and will never be serialized. No need to register it to pass factory

Debug::Debug(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
    , m_framebuffer(std::make_unique<gpu::Framebuffer>(width, height))
{
    Debug::Init();
}

DebugShapeHandle Debug::DrawDebugBox(
        const DebugShapeHandle &index,
        const glm::vec3 &center,
        const glm::vec3 &extent,
        const glm::vec3 &color,
        const glm::mat4 &transform)
{
    if (m_debugVertices.has_key(index))
    {
        EditBox(index, center, extent, color, transform);
        return index;
    }

    return RegisterBox(center, extent, color, transform);
}

DebugShapeHandle Debug::DrawDebugSphere(
        const DebugShapeHandle &index,
        const glm::vec3 &center,
        float radius,
        const glm::vec3 &color,
        const glm::mat4 &transform)
{
    if (m_debugVertices.has_key(index))
    {
        EditSphere(index, center, radius, color, transform);
        return index;
    }

    return RegisterSphere(center, radius, color, transform);
}

void Debug::AddLine(std::vector<gfx::DebugVertex>& shape, const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    shape.emplace_back(start, color);
    shape.emplace_back(end, color);
}

void Debug::EditLine(const DebugShapeHandle& shapeId, int index, const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    m_debugVertices.get(shapeId)->at(index) = {start, color};
    m_debugVertices.get(shapeId)->at(index + 1) = {end, color};
}

DebugShapeHandle Debug::RegisterBox(
        const glm::vec3 &center,
        const glm::vec3 &extent,
        const glm::vec3 &color,
        const glm::mat4 &transform)
{
    glm::vec3 min = transform * glm::vec4(center - extent, 1.f);
    glm::vec3 max = transform * glm::vec4(center + extent, 1.f);

    std::vector<gfx::DebugVertex> shape;
    // top face
    AddLine(shape, {max}, {min.x, max.y, max.z}, color);
    AddLine(shape, {min.x, max.y, max.z}, {min.x, max.y, min.z}, color);
    AddLine(shape, {min.x, max.y, min.z}, {max.x, max.y, min.z}, color);
    AddLine(shape, {max.x, max.y, min.z}, {max}, color);

    // bottom face
    AddLine(shape, {min}, {max.x, min.y, min.z}, color);
    AddLine(shape, {max.x, min.y, min.z}, {max.x, min.y, max.z}, color);
    AddLine(shape, {max.x, min.y, max.z}, {min.x, min.y, max.z}, color);
    AddLine(shape, {min.x, min.y, max.z}, {min}, color);

    // sides
    AddLine(shape, {min}, {min.x, max.y, min.z}, color);
    AddLine(shape, {min.x, min.y, max.z}, {min.x, max.y, max.z}, color);
    AddLine(shape, {max.x, min.y, min.z}, {max.x, max.y, min.z}, color);
    AddLine(shape, {max.x, min.y, max.z}, {max}, color);

    m_dirty = true;
    return m_debugVertices.emplace(std::move(shape));
}

void Debug::EditBox(
        const DebugShapeHandle& index,
        const glm::vec3 &center,
        const glm::vec3 &extent,
        const glm::vec3 &color,
        const glm::mat4 &transform)
{
    glm::vec3 min = transform * glm::vec4(center - extent, 1.f);
    glm::vec3 max = transform * glm::vec4(center + extent, 1.f);

    // top face
    EditLine(index, 0,  {max}, {min.x, max.y, max.z}, color);
    EditLine(index, 2, {min.x, max.y, max.z}, {min.x, max.y, min.z}, color);
    EditLine(index, 4, {min.x, max.y, min.z}, {max.x, max.y, min.z}, color);
    EditLine(index, 6, {max.x, max.y, min.z}, {max}, color);

    // bottom face
    EditLine(index, 8, {min}, {max.x, min.y, min.z}, color);
    EditLine(index, 10, {max.x, min.y, min.z}, {max.x, min.y, max.z}, color);
    EditLine(index, 12, {max.x, min.y, max.z}, {min.x, min.y, max.z}, color);
    EditLine(index, 14, {min.x, min.y, max.z}, {min}, color);

    // sides
    EditLine(index, 16, {min}, {min.x, max.y, min.z}, color);
    EditLine(index, 18, {min.x, min.y, max.z}, {min.x, max.y, max.z}, color);
    EditLine(index, 20, {max.x, min.y, min.z}, {max.x, max.y, min.z}, color);
    EditLine(index, 22, {max.x, min.y, max.z}, {max}, color);

    m_dirty = true;
}

DebugShapeHandle Debug::RegisterSphere(const glm::vec3 &center, float radius, const glm::vec3 &color, const glm::mat4 &transform)
{
    std::vector<gfx::DebugVertex> shape;
    // create the vertices first, then iterate on those to create the lines
    std::array<glm::vec3, 16> vertices;
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = transform * glm::vec4(std::cos(i * glm::two_pi<float>() / 16.f), .0f, std::sin(i * glm::two_pi<float>() / 16.f), 1);
    }

    AddLine(shape, vertices[0], vertices[1], color);
    for (int i = 1; i < vertices.size() - 1; i++)
    {
        AddLine(shape, vertices[i], vertices[i + 1], color);
    }
    // add last vertex by hand because i don't like modulos everywhere
    AddLine(shape, vertices[vertices.size() - 1], vertices[0], color);

    // second loop
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = transform * glm::vec4(.0f, std::cos(i * glm::two_pi<float>() / 16.f), std::sin(i * glm::two_pi<float>() / 16.f), 1);
    }

    AddLine(shape, vertices[0], vertices[1], color);
    for (int i = 1; i < vertices.size() - 1; i++)
    {
        AddLine(shape, vertices[i], vertices[i + 1], color);
    }
    // add last vertex by hand because i don't like modulos everywhere
    AddLine(shape, vertices[vertices.size() - 1], vertices[0], color);

    return m_debugVertices.emplace(std::move(shape));
}

void Debug::EditSphere(
        const DebugShapeHandle& index,
        const glm::vec3 &center,
        float radius,
        const glm::vec3 &color,
        const glm::mat4 &transform)
{
    // create the vertices first, then iterate on those to create the lines
    std::array<glm::vec3, 16> vertices {};
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = transform * glm::vec4(std::cos(i * glm::two_pi<float>() / 16.f) * radius, .0f, std::sin(i * glm::two_pi<float>() / 16.f) * radius, 1);
    }

    EditLine(index, 0, vertices[0], vertices[1], color);
    for (int i = 1; i < vertices.size() - 1; i++)
    {
        EditLine(index,i * 2, vertices[i], vertices[i + 1], color);
    }
    // add last vertex by hand because i don't like modulos everywhere
    EditLine(index, (vertices.size() - 1) * 2, vertices[vertices.size() - 1], vertices[0], color);

    // second loop
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = transform * glm::vec4(.0f, std::cos(i * glm::two_pi<float>() / 16.f) * radius, std::sin(i * glm::two_pi<float>() / 16.f) * radius, 1);
    }

    EditLine(index, vertices.size() * 2, vertices[0], vertices[1], color);
    for (int i = 1; i < vertices.size() - 1; i++)
    {
        EditLine(index, vertices.size() * 2 + i * 2, vertices[i], vertices[i + 1], color);
    }
    // add last vertex by hand because i don't like modulos everywhere
    EditLine(index, (vertices.size() * 2 - 1) * 2, vertices[vertices.size() - 1], vertices[0], color);
}

std::vector<gfx::DebugVertex> Debug::PrepareData()
{
    std::vector<gfx::DebugVertex> packedVertices;
    for (const auto& shape : m_debugVertices)
    {
        for (const auto& vertex : shape)
            packedVertices.push_back(vertex);
    }

    return packedVertices;
}

void Debug::DrawGrid()
{
    m_shouldDrawGridNextFrame = true;
}

void Debug::Init()
{
    gpu::TexturePtr albedo = ResourcesManager::Instance()->GetTexture(RendererManager::RENDERED_FRAME_NAME);
    m_framebuffer->Attach(gpu::Framebuffer::Color, albedo, 0);

    std::vector<gpu::Shader::ShaderSource> sources = {
        {gpu::Shader::Vertex, "shaders/debug.vert"},
        {gpu::Shader::Fragment, "shaders/debug.frag"}
    };
    ResourcesManager::Instance()->CacheShader(DEBUG_SHADER_NAME, sources);

    std::vector<gpu::Shader::ShaderSource> grid = {
        {gpu::Shader::Vertex, "shaders/grid.vert"},
        {gpu::Shader::Fragment, "shaders/grid.frag"},
    };
    ResourcesManager::Instance()->CacheShader(GRID_SHADER_NAME, grid);
}

void Debug::Render(const FrameData &frameData)
{
    if (m_debugVertices.empty())
        return;

    if (frameData.profilerGPU)
        frameData.profilerGPU->BeginScope("Debug");

    m_framebuffer->Bind(gpu::Framebuffer::ReadWrite);

    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);

    // draw grid
    if (m_shouldDrawGridNextFrame)
    {
        gpu::ShaderPtr grid = ResourcesManager::Instance()->GetShader(GRID_SHADER_NAME);
        grid->Bind();

        gpu::TexturePtr depth = ResourcesManager::Instance()->GetTexture(GBuffer::GBUFFER_DEPTH_NAME);
        gpu::TexturePtr res = ResourcesManager::Instance()->GetTexture(RendererManager::RENDERED_FRAME_NAME);

        depth->Bind(0);
        res->Bind(1);

        ResourcesManager::Instance()->GetMesh(ResourcesManager::DEFAULT_PLANE_NAME)->Draw();

        //m_shouldDrawGridNextFrame = false;
    }

    // draw guizmos
    gpu::ShaderPtr shader = ResourcesManager::Instance()->GetShader(DEBUG_SHADER_NAME);
    shader->Bind();

    // pack our vertex datas in a buffer
    if (m_dirty)
    {
        std::vector<gfx::DebugVertex> data = PrepareData();
        m_lineRenderer.SetData(data);
    }

    m_lineRenderer.Draw();

    m_framebuffer->Unbind(gpu::Framebuffer::ReadWrite);

    glDisable(GL_BLEND);

    if (frameData.profilerGPU)
        frameData.profilerGPU->EndScope("Debug");
}

void Debug::RenderUI()
{
}

void Debug::Rebuild(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
    m_framebuffer->SetSize(width, height);
}

void Debug::Serialize(YAML::Node passes)
{
    // never serialized
}

void Debug::Deserialize(YAML::Node pass)
{
    // never deserialized
}
} // lum::rdr