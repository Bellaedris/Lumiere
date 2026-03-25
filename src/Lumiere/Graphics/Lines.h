//
// Created by belle on 23/03/2026.
//

#pragma once
#include <memory>

#include "Lumiere/GPU/Buffer.h"
#include "Lumiere/GPU/VAO.h"

namespace lum::gfx
{
struct DebugVertex
{
    glm::vec3 position;
    glm::vec3 color;
    DebugVertex(const glm::vec3& position, const glm::vec3& color) : position(position), color(color) {}
};

/**
 * \brief A collection of lines that are uploaded frequently, mostly for our debug pass
 */
class Lines
{
private:
    #pragma region Members
    gpu::Buffer m_buffer;
    gpu::Vao m_vao;

    uint32_t m_vertexCount {0};
    #pragma endregion // Members
public:
    Lines();

    /**
     * \brief Change the data we want to render
     * \param vertices a collection of vertices that form the lines to draw
     */
    void SetData(std::vector<DebugVertex>& vertices);

    void Draw() const;
};
} // lum::gfx