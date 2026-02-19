//
// Created by Bellaedris on 12/12/2025.
//

#include "App.h"
#include "../LumiereConfig.h"
#include "GPU/Framebuffer.h"

namespace lum
{
    App::App(int width, int height, int majorVersionHint/* = 4 */, int minorVersionHint/* = 6 */)
            : m_window(std::make_shared<Window>(width, height, majorVersionHint, minorVersionHint))
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

        while(m_window->ShouldClose() == false)
        {
            PreRender();

            Render();
            RenderUI();

            m_window->SwapBuffers();
            m_window->PollEvents();
            m_frameIndex++;
        }

        Cleanup();
    }

    void App::PreRender()
    {
        m_deltaTime = m_deltaTimer.Elapsed();
        // If there are things you want to do before rendering ...
        glm::vec3 inputDir = m_window->GatherInput();
        m_camera->ProcessMouseMovement(m_window->GetMousePosOffset());
        m_camera->UpdatePosition(inputDir, m_deltaTime);

        // update dt
        m_deltaTimer.Reset();

        // Initialise a new frame

    }
}
