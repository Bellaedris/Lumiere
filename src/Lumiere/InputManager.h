//
// Created by Bellaedris on 20/02/2026.
//

#pragma once
#include <map>
#include <glm/glm.hpp>

namespace lum
{
enum class KeyCode
{
    lKeyUnknown,
    lKeyW,
    lKeyA,
    lKeyS,
    lKeyD,
    lKeyQ,
    lKeyE,
    lKeyAlt,
    lKeySpace,
    lKeyCtrl,
    lKeyShift,
    lKeyEsc
};

enum class MouseButton
{
    lUnknownButton,
    lLeftClick,
    lRightClick,
    lMiddleClick
    // eventually add additional mouse buttons
};

class InputManager
{
private:
    struct KeyState
    {
        bool isDown {false};
        bool downLastFrame {false};
    };

    inline static std::map<KeyCode, KeyState>     keyStates;
    inline static std::map<MouseButton, KeyState> mouseButtonStates;
    /**
     * \brief Mouse movement recorded each frame by the window
     */
    inline static glm::vec2 m_axis {.0f, .0f};

    friend class Window;

public:
    static bool IsKeyDown(KeyCode key) { return keyStates[key].isDown; }
    static bool IsKeyPressed(KeyCode key) { return keyStates[key].isDown && !keyStates[key].downLastFrame; }
    static bool IsKeyReleased(KeyCode key) { return !keyStates[key].isDown && keyStates[key].downLastFrame; }

    static bool IsMouseButtonDown(MouseButton button) { return mouseButtonStates[button].isDown; }
    static bool IsMouseButtonPressed(MouseButton button) { return mouseButtonStates[button].isDown && !mouseButtonStates[button].downLastFrame; }
    static bool IsMouseButtonReleased(MouseButton button) { return !mouseButtonStates[button].isDown && mouseButtonStates[button].downLastFrame; }

    static glm::vec2 GetAxis() { return m_axis; };

    static void Update();
};
} // lum