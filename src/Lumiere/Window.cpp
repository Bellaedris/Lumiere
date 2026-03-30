//
// Created by Bellaedris on 11/12/2025.
//

#include "Window.h"

#include <ranges>

#include "VectorUtils.h"
#include <stdexcept>

#include "Events/RenderEvents.h"

namespace lum
{
    Window::Window(int width, int height, int major, int minor, std::shared_ptr<evt::EventHandler> events)
        : m_width(width)
        , m_height(height)
        , m_events(events)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(width, height, "Lumiere", NULL, NULL);
        if (m_window == nullptr)
        {
            glfwTerminate();
            throw std::runtime_error("Couldn't create GLFW window");
        }
        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            throw std::runtime_error("Couldn't initialize GLAD");
        }

        glfwSetFramebufferSizeCallback(m_window, Window::framebuffer_size_callback);
        glfwSetCursorPosCallback(m_window, Window::mouse_callback);
        glfwSetMouseButtonCallback(m_window, Window::mouse_button_callback);
        glfwSetKeyCallback(m_window, Window::key_callback);

        glfwSetWindowUserPointer(m_window, this);

        glfwMaximizeWindow(m_window);
    }

    bool Window::ShouldClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::PollEvents()
    {
        // update the input manager
        for (auto &[isDown, downLastFrame] : InputManager::keyStates | std::views::values)
            downLastFrame = isDown;

        for (auto& [isDown, downLastFrame] : InputManager::mouseButtonStates | std::views::values)
            downLastFrame = isDown;

        InputManager::m_axis = {m_offsetX, m_offsetY};
        // reset once consume, or we always have movement, which is quite troublesome
        m_offsetX = 0;
        m_offsetY = 0;

        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_window);
    }

    void Window::framebuffer_size_callback(GLFWwindow *w, int width, int height)
    {
        glViewport(0, 0, width, height);
        Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(w));
        instance->m_width = width;
        instance->m_height = height;

        // signal that the viewport size just changed
        instance->m_events->Emit(std::make_unique<evt::WindowResizedEvent>());
    }

    void Window::mouse_callback(GLFWwindow *w, double xpos, double ypos)
    {
        Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(w));
        // update the offset between last frame and the current frame
        instance->UpdateMouseOffset(xpos, ypos);
    }

    void Window::mouse_button_callback(GLFWwindow *w, int button, int action, int mods)
    {
        MouseButton lButton = GLFWMouseButtonToMouseButton(button);
        InputManager::mouseButtonStates[lButton].isDown = action == GLFW_PRESS;
    }

    void Window::key_callback(GLFWwindow *w, int key, int scancode, int action, int mods)
    {
        KeyCode lKey = GLFWKeyToKeyCode(key);

        // here, we can have PRESS, RELEASE or REPEAT, so I can't just go the ternary path.
        if (action == GLFW_PRESS)
            InputManager::keyStates[lKey].isDown = true;
        if (action == GLFW_RELEASE)
            InputManager::keyStates[lKey].isDown = false;
    }

    KeyCode Window::GLFWKeyToKeyCode(int key)
    {
        switch (key)
        {
            case GLFW_KEY_W:            return KeyCode::lKeyW;
            case GLFW_KEY_A:            return KeyCode::lKeyA;
            case GLFW_KEY_S:            return KeyCode::lKeyS;
            case GLFW_KEY_D:            return KeyCode::lKeyD;
            case GLFW_KEY_Q:            return KeyCode::lKeyQ;
            case GLFW_KEY_E:            return KeyCode::lKeyE;
            case GLFW_KEY_LEFT_ALT:     return KeyCode::lKeyAlt;
            case GLFW_KEY_LEFT_CONTROL: return KeyCode::lKeyCtrl;
            case GLFW_KEY_LEFT_SHIFT:   return KeyCode::lKeyShift;
            case GLFW_KEY_SPACE:        return KeyCode::lKeySpace;
            default:                    return KeyCode::lKeyUnknown;
        }
    }

    MouseButton Window::GLFWMouseButtonToMouseButton(int button)
    {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::lLeftClick;
            case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::lRightClick;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::lMiddleClick;
            default:                       return MouseButton::lUnknownButton;
        }
    }

    void Window::UpdateMouseOffset(double x, double y)
    {
        if(m_mouseLostFocus)
        {
            m_lastX = x;
            m_lastY = y;
            m_mouseLostFocus = false;
        }

        m_offsetX = x - m_lastX;
        m_offsetY = m_lastY - y;

        m_lastX = x;
        m_lastY = y;

        if (std::abs(m_offsetX) < 1.f) m_offsetX = .0f;
        if (std::abs(m_offsetY) < 1.f) m_offsetY = .0f;
    }
}
