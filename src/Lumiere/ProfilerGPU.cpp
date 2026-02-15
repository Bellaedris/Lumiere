//
// Created by Bellaedris on 13/02/2026.
//

#include "ProfilerGPU.h"

namespace lum
{
ProfilerGPU::ProfilerGPU()
    : m_frameStats(MAX_FRAMES_STORAGE)
{}

void ProfilerGPU::BeginScope(const std::string &handle)
{
    // if the query does not exist, create it
    if (m_handlesToTimers.find(handle) == m_handlesToTimers.end())
    {
        std::vector<gpu::GPUTimer> timers(MAX_FRAME_IN_FLIGHT);
        m_handlesToTimers.emplace(handle, std::move(timers));
    }

    m_frameStats[m_currentFrame].scopeHandles.push_back(handle);
    m_handlesToTimers[handle][m_currentFrame % MAX_FRAME_IN_FLIGHT].Begin();
}

void ProfilerGPU::EndScope(const std::string &handle)
{
    m_handlesToTimers[handle][m_currentFrame % MAX_FRAME_IN_FLIGHT].End();
}

void ProfilerGPU::BeginFrame()
{
    FrameStats& currentFrame = m_frameStats[m_currentFrame];
    //currentFrame.elapsedTimes.clear();
    currentFrame.scopeHandles.clear();
}

void ProfilerGPU::EndFrame()
{
    // query the results of the n - 2 frame
    FrameStats& bufferedFrame = m_frameStats[m_lastFrame];
    bufferedFrame.elapsedTimes.clear();

    for (int i = 0; i < bufferedFrame.scopeHandles.size(); i++)
    {
        bufferedFrame.elapsedTimes.push_back(m_handlesToTimers[bufferedFrame.scopeHandles[i]][m_lastFrame % MAX_FRAME_IN_FLIGHT].Elapsed());
    }

    //update the frame counts
    m_lastFrame = m_previousFrame;
    m_previousFrame = m_currentFrame;

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_STORAGE;
}
} // lum