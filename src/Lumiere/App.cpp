//
// Created by Bellaedris on 12/12/2025.
//

#include "App.h"
#include "../LumiereConfig.h"
#include "GPU/Framebuffer.h"

namespace lum
{
    App::App(int width, int height, int majorVersionHint/* = 4 */, int minorVersionHint/* = 6 */)
            : m_window(width, height, majorVersionHint, minorVersionHint)
    {
        #ifndef NDEBUG
        vendor = glGetString(GL_VENDOR);
        gpu = glGetString(GL_RENDERER);
        api = glGetString(GL_VERSION);
        #endif
    }

    void App::Run()
    {
        Init();
        InitUI();

        while(m_window.ShouldClose() == false)
        {
            PreRender();

            m_gpuDeltaTime.Begin();
            Render();
            m_gpuDeltaTime.End();
            RenderUI();

            m_window.SwapBuffers();
            m_window.PollEvents();
        }

        Cleanup();
    }

    void App::PreRender()
    {
        m_deltaTime = m_deltaTimer.Elapsed();
        // If there are things you want to do before rendering ...
        glm::vec3 inputDir = m_window.GatherInput();
        m_camera->ProcessMouseMovement(m_window.GetMousePosOffset());
        m_camera->UpdatePosition(inputDir, m_deltaTime);

        // update dt
        m_deltaTimer.Reset();
    }

    void App::RenderUI()
    {
        // Initialise a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // the actual UI goes here
        #ifndef NDEBUG
        // debug-only view
        ImGui::SetNextWindowPos({0, 0}, 0, {0, 0});
        ImGui::Begin(
                "Infos",
                nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize
        );
        {
            ImGui::Text("Lumiere version %d.%d", LUMIERE_VERSION_MAJOR, LUMIERE_VERSION_MINOR);
            ImGui::Text("OpenGL%s", api);
            ImGui::Text("%s %s", vendor, gpu);

            float average = 0.0f;
            for (float n: framerate)
                average += n;
            average /= (float) IM_ARRAYSIZE(framerate);
            ImGui::Text("Average %02fFPS", average);
            uint64_t elapsedGPU = m_gpuDeltaTime.Elapsed();
            // ugly way to convert to a readable format but I'm kinda lazy and don't want to fight both
            // std::chrono types and c strings
            float cpuSeconds = m_deltaTime * 1000.f;
            int milli = (int) (elapsedGPU / 1000000);
            int micro = (int) ((elapsedGPU / 1000) % 1000);
            ImGui::Text(
                    "cpu %03fms\ngpu %02dms % 03dus",
                    cpuSeconds,
                    milli, micro
            );
            ImGui::End();
        }
        #endif

        // render the UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void App::Cleanup()
    {
        // cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void App::InitUI()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        ImGui_ImplGlfw_InitForOpenGL(m_window.GetWindow(), true);
        ImGui_ImplOpenGL3_Init();
    }
}
