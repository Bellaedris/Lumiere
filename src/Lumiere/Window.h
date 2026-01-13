//
// Created by Bellaedris on 11/12/2025.
//

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace lum
{
    class Window
    {
    private:
        static void framebuffer_size_callback(GLFWwindow *w, int width, int height);
        static void mouse_callback(GLFWwindow *w, double xpos, double ypos);

        GLFWwindow *m_window;

        int m_width;
        int m_height;

        // mouse pos
        float m_lastX {0};
        float m_lastY {0};
        float m_offsetX {0};
        float m_offsetY {0};
        bool m_mouseLostFocus {true};

        void UpdateMouseOffset(double x, double y);

    public:
        Window(int width, int height, int major, int minor);

        bool ShouldClose();
        void PollEvents();
        void SwapBuffers();

        glm::vec3 GatherInput();

        [[nodiscard]] inline int const Width() const { return m_width; };
        [[nodiscard]] inline int const Height() const { return m_height; };
        [[nodiscard]] inline float const AspectRatio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); };
        [[nodiscard]] inline GLFWwindow* const GetWindow() const { return m_window; };
        [[nodiscard]] inline glm::vec2 GetMousePosOffset() const { return {m_offsetX, m_offsetY}; };
    };
}