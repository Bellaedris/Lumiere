//
// Created by Bellaedris on 12/12/2025.
//

#include "App.h"
#include "../LumiereConfig.h"

namespace lum
{
    App::App(int width, int height, int majorVersionHint/* = 4 */, int minorVersionHint/* = 6 */)
        : m_internalEvents(std::make_shared<evt::EventHandler>())
        , m_window(std::make_shared<Window>(width, height, majorVersionHint, minorVersionHint, m_internalEvents))
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
            m_window->PollEvents();
            PreRender();

            Render();
            RenderUI();

            m_window->SwapBuffers();
            m_frameIndex++;
        }
    }

    void App::PreRender()
    {
        m_deltaTime = m_deltaTimer.Elapsed();
        // update dt
        m_deltaTimer.Reset();
    }
}
