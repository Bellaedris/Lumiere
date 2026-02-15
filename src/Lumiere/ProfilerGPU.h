//
// Created by Bellaedris on 13/02/2026.
//

#pragma once
#include <map>

#include "GPU/Timer.h"

namespace lum
{
class ProfilerGPU
{
public:
    struct FrameStats
    {
        std::vector<std::string> scopeHandles;
        std::vector<uint64_t> elapsedTimes;
    };
private:
    std::map<std::string, std::vector<gpu::GPUTimer>> m_handlesToTimers;
    std::vector<FrameStats> m_frameStats;

    int m_currentFrame {2};
    int m_previousFrame{1};
    int m_lastFrame    {0};

    constexpr static int MAX_FRAME_IN_FLIGHT = 3;
public:
    constexpr static int MAX_FRAMES_STORAGE = 100;

    ProfilerGPU();

    void BeginScope(const std::string& handle);
    void EndScope(const std::string& handle);

    void BeginFrame();
    void EndFrame();

    const std::vector<FrameStats>& Data() const { return m_frameStats; };
};
} // lum