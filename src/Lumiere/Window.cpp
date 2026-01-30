//
// Created by Bellaedris on 11/12/2025.
//

#include "Window.h"
#include "VectorUtils.h"
#include <stdexcept>

namespace lum
{
    Window::Window(int width, int height, int major, int minor)
        : m_width(width)
        , m_height(height)
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

        glfwSetWindowUserPointer(m_window, this);
    }

    bool Window::ShouldClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::PollEvents()
    {
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
    }

    void Window::mouse_callback(GLFWwindow *w, double xpos, double ypos)
    {
        Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(w));
        // Camera movement is only tracked when pressing LEFT ALT
        if(glfwGetKey(w, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        {
            glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // update the offset between last frame and the current frame
            instance->UpdateMouseOffset(xpos, ypos);
        }
        else
            instance->UpdateMouseOffset(instance->m_lastX, instance->m_lastY);
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
    }

    glm::vec3 Window::GatherInput()
    {
        if(glfwGetKey(m_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // update the offset between last frame and the current frame
        }
        else
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_mouseLostFocus = true;
        }

        // get the input axes by reading pressed keys
        glm::vec3 dir(0, 0, 0);
        if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            dir += VectorUtils::FORWARD;
        if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            dir -= VectorUtils::RIGHT;
        if(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            dir -= VectorUtils::FORWARD;
        if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            dir += VectorUtils::RIGHT;
        if(glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
            dir -= VectorUtils::UP;
        if(glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
            dir += VectorUtils::UP;

        return dir;
    }
}