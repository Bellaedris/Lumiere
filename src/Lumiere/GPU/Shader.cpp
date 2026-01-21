//
// Created by Bellaedris on 12/12/2025.
//

#include <iostream>
#include "Shader.h"
#include "../Utils/FileUtils.h"

namespace lum::gpu
{
    GLuint Shader::GetShaderType(Shader::ShaderType type)
    {
        switch(type)
        {
            case Vertex:
                return GL_VERTEX_SHADER;
            case Fragment:
                return GL_FRAGMENT_SHADER;
            case Compute:
                return GL_COMPUTE_SHADER;
            default:
                std::cerr << "Invalid shader type";
                return GL_FALSE;
        }
    }

    Shader::Shader()
        : m_program(glCreateProgram())
    {}

    Shader::~Shader()
    {
        glDeleteProgram(m_program);
    }

    Shader::Shader(Shader &&other) noexcept
        : m_program(other.m_program)
    {
        other.m_program = 0;
    }

    Shader &Shader::operator=(Shader &&other) noexcept
    {
        if (this != &other)
        {
            glDeleteProgram(m_program);
            m_program       = other.m_program;
            other.m_program = 0;
        }
        return *this;
    }

    void Shader::AddShaderFromFile(const ShaderSource &shaderInfo)
    {
        if(m_type == ShaderType::Compute)
        {
            std::cerr << "A compute program should only contain a compute shader\n";
            return;
        }

        if(shaderInfo.type == ShaderType::Compute && m_type != ShaderType::None)
        {
            std::cerr << "A compute program must contain a single compute shader\n";
            return;
        }

        m_type = shaderInfo.type;

        std::optional<std::string> shaderData = utils::FileUtils::read_file(shaderInfo.path.c_str());
        if(shaderData.has_value() == false)
        {
            std::cerr << "Couldn't open file " << shaderInfo.path << "\n";
            return;
        }

        const char*  shaderSource = shaderData->c_str();
        unsigned int shader       = glCreateShader(GetShaderType(shaderInfo.type));
        glShaderSource(shader, 1, &shaderSource, nullptr);
        glCompileShader(shader);

        // check for compilation errors
        int  success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cout << "error compiling shaders at " << shaderInfo.path << ": " << infoLog << std::endl;
            return;
        }

        glAttachShader(m_program, shader);

        glDeleteShader(shader);
        m_shaderSources.emplace_back(shaderInfo.type, std::string(shaderInfo.path));
    }

    void Shader::Create()
    {
        glLinkProgram(m_program);
        int  success;
        char infoLog[512];
        glGetShaderiv(m_program, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(m_program, 512, nullptr, infoLog);
            std::cerr << "error linking program: " << infoLog << std::endl;
        }
        else
            m_created = true;
    }

    void Shader::Reload()
    {
        glDeleteProgram(m_program);
        m_program = glCreateProgram();
        m_type = ShaderType::None;

        std::vector<ShaderSource> sourceToReload = m_shaderSources;
        m_shaderSources.clear();
        for (const ShaderSource& source : sourceToReload)
            AddShaderFromFile(source);

        Create();
    }

    void Shader::Bind()
    {
        if(m_created == false)
            Create();
        glUseProgram(m_program);
    }

    void Shader::Dispatch(uint32_t x, uint32_t y, uint32_t z)
    {
        if(m_type != ShaderType::Compute)
        {
            std::cerr << "Cannot dispatch a non-compute shader\n";
            return;
        }
        glDispatchCompute(x, y ,z);
    }

    void Shader::Wait()
    {
        if(m_type != ShaderType::Compute)
        {
            std::cerr << "Cannot dispatch a non-compute shader\n";
            return;
        }
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
} // mgl