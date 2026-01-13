//
// Created by Bellaedris on 14/12/2025.
//

#pragma once

namespace lum
{
class VectorUtils
{
public:
    static constexpr glm::vec3 FORWARD {.0f, .0f, 1.f};
    static constexpr glm::vec3 UP { .0f, 1.f, .0f};
    static constexpr glm::vec3 RIGHT { 1.f, .0f, .0f};

    static constexpr float EPSILON = .0001f;
};
}