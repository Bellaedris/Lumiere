//
// Created by belle on 07/04/2026.
//

#pragma once

namespace lum::rdr
{
/**
 * \brief Contains all the Renderer settings that can be modified through gameplay. Never serialized, only used to apply gameplay-time
 * rendering changes
 */
struct RenderSettings
{
    bool m_shouldSwitchPipeline {false};
    int m_activePipeline {0};
    float m_cameraSensorIso {1.f};
};
}