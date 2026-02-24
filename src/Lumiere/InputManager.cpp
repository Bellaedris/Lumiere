//
// Created by Bellaedris on 20/02/2026.
//

#include "InputManager.h"

namespace lum {
void InputManager::Update()
{
    for (auto& pair : keyStates)
    {
        KeyState& state = pair.second;
        if (state.isDown)
            state.downLastFrame = true;
        else
            state.downLastFrame = false;
    }

    for (auto& pair : mouseButtonStates)
    {
        KeyState& state = pair.second;
        if (state.isDown)
            state.downLastFrame = true;
        else
            state.downLastFrame = false;
    }
}
} // lum