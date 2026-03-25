//
// Created by belle on 23/03/2026.
//

#include "Lines.h"

namespace lum
{
gfx::Lines::Lines()
    : m_buffer(gpu::Buffer::Vertex)
{
}

void gfx::Lines::SetData(std::vector<DebugVertex>& vertices)
{
    m_vertexCount = vertices.size();
    m_vao.Bind();
    m_buffer.Bind();

    m_buffer.Write(sizeof(DebugVertex) * vertices.size(), vertices.data(), gpu::Buffer::DynamicDraw);

    m_vao.SetAttribute(0, gpu::GLUtils::Float, 0, 3, sizeof(DebugVertex));
    m_vao.SetAttribute(1, gpu::GLUtils::Float, offsetof(DebugVertex, color), 3, sizeof(DebugVertex));

    m_vao.Unbind();
}

void gfx::Lines::Draw() const
{
    if (m_vertexCount <= 0)
        return;

    m_vao.Bind();
    glDrawArrays(GL_LINES, 0, m_vertexCount);
    m_vao.Unbind();
}
} // lum