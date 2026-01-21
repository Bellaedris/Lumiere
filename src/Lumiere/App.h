//
// Created by Bellaedris on 12/12/2025.
//

#pragma once

#include <memory>
#include "Window.h"
#include "Renderer/Camera.h"
#include "GPU/Shader.h"
#include "Graphics/Mesh.h"
#include "GPU/Framebuffer.h"
#include "GPU/Timer.h"
#include "GPU/Texture.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace lum
{
    class App
    {
    protected:
        #pragma region Members
        Window m_window;

        std::unique_ptr<rdr::Camera> m_camera {nullptr};

        gpu::Timer<std::chrono::seconds> m_deltaTimer;
        gpu::GPUTimer m_gpuDeltaTime;
        float m_deltaTime;

        // analytics, debug only
        #ifndef NDEBUG
        float m_frameTime;
        const GLubyte* vendor; // GPU vendor
        const GLubyte* gpu; // GPU model
        const GLubyte* api;
        float framerate[100] = {};
        int values_offset = 0;
        #endif
        #pragma endregion Members

        /**
         * \brief Initializes the application
         */
        virtual void Init() = 0;

        /**
         * \brief Dear ImGui boilerplate
         */
        void InitUI();

        /**
         * \brief Called each frame before rendering
         */
        void PreRender();

        /**
         * \brief Called each frame, Draws to the screen
         */
        virtual void Render() = 0;

        /**
         * \brief Called each frame, draws ImGui UI to the screen
         */
        void RenderUI();

        /**
         * \brief Called before quitting the application
         */
        void Cleanup();

    public:
        App(int width, int height, int majorVersionHint = 4, int minorVersionHint = 6);

        /**
         * \brief Runs the application until it is quited
         */
        void Run();
    };
}
