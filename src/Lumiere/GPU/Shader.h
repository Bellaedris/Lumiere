//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

namespace lum::gpu
{
class Shader
{
public:
    #pragma region Enum
    enum ShaderType
    {
        Vertex,
        Fragment,
        Compute,
        None
    };
    #pragma endregion Enum

    #pragma region Struct
    struct ShaderSource
    {
        ShaderType type;
        std::string path;
    };
    #pragma endregion Struct
private:
    #pragma region Members
    uint32_t m_program;
    bool m_created {false};
    ShaderType m_type {ShaderType::None};

    std::vector<ShaderSource> m_shaderSources;
    std::map<std::string, int> uniformLocationCache;
    #pragma endregion Members

    GLuint GetShaderType(ShaderType type);
public:
    /**
     * \brief Creates an OpenGL program
     */
    Shader();
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    #pragma region Methods
    /**
     * \brief Add a shader to the shader program. Note that compute shader programs can only contain a single shader.
     * Other programs can contain as many non-compute programs. The usual workflow would be to either add a vertex,
     * then fragment shader, or to add a single compute.
     * \param shaderInfo
     */
    void AddShaderFromFile(const ShaderSource &shaderInfo);

    /**
     * \brief Finalizes the creation of the shader after adding all the individual shaders we wanted.
     */
    void Create();

    /**
     * \brief Recreate the program entirely. Equivalent to creating a new shader and calling the same AddShaderFromFile
     * calls that were used to create the shader.
     */
    void Reload();

    /**
     * \brief Bind the shader for use
     */
    void Bind();

    /**
     * \brief Compute shaders only. Dispatches a compute shader using work groups of size x * y * z.
     * Work group sizes in all dimensions should be a multiple of 32 (Nvidia GPUs have warps of size 32, Amd of size 64).
     * For instance, using a size of 1 * 1 * 1 on an nvidia GPU will result in an entire warp being dispatched, but only 1 thread
     * \param x number of work groups in the X dimension
     * \param y number of work groups in the Y dimension
     * \param z number of work groups in the Z dimension
     */
    void Dispatch(uint32_t x, uint32_t y, uint32_t z);

    /**
     * \brief Wait for a compute shader execution to finish. Does nothing on a non-compute shader
     */
    void Wait();
    #pragma endregion Methods

    #pragma region Uniform data
    int GetLocation(const std::string &name)
    {
        if(uniformLocationCache.contains(name) == false)
            uniformLocationCache.emplace(std::string(name), glGetUniformLocation(m_program, name.c_str()));

        return uniformLocationCache[name];
    }

    void UniformData(const std::string& name, const glm::mat4 mat)
    {
        glUniformMatrix4fv(GetLocation(name), 1, false, glm::value_ptr(mat));
    }

    void UniformData(const std::string& name, const float x, const float y, const float z, const float w)
    {
        glUniform4f(GetLocation(name), x, y, z, w);
    }

    void UniformData(const std::string& name, const glm::vec4& data)
    {
        glUniform4f(GetLocation(name), data.x, data.y, data.z, data.w);
    }

    void UniformData(const std::string& name, const glm::vec3& data)
    {
        glUniform3f(GetLocation(name), data.x, data.y, data.z);
    }

    void UniformData(const std::string& name, const glm::vec2& data)
    {
        glUniform2f(GetLocation(name), data.x, data.y);
    }

    void UniformData(const std::string &name, const float x, const float y, const float z)
    {
        glUniform3f(GetLocation(name), x, y, z);
    }

    void UniformData(const std::string& name, const float x)
    {
        glUniform1f(GetLocation(name), x);
    }

    void UniformData(const std::string& name, const int x)
    {
        glUniform1i(GetLocation(name), x);
    }
    #pragma endregion Uniform data
};

using ShaderPtr = std::shared_ptr<Shader>;
} // lum