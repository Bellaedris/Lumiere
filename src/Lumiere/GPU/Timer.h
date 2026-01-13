//
// Created by Bellaedris on 15/12/2025.
//

#pragma once

#include <chrono>
#include <iostream>

namespace lum::gpu
{
    // concepts to only allow chrono-derived types, thanks https://stackoverflow.com/questions/74383254/concept-that-models-only-the-stdchrono-duration-types
    template <class _Tp>
    struct is_chrono_duration : std::false_type {};

    template <class _Rep, class _Period>
    struct is_chrono_duration<std::chrono::duration<_Rep, _Period>> : std::true_type {};

    template<typename _Tp>
    concept chrono_duration = is_chrono_duration<_Tp>::value;

    /**
     * \brief Inspired by a video of TheCherno, just a quick utility to wrap timers
     */
    template<chrono_duration T>
    class Timer
    {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    public:
        Timer() { Reset(); };
        void Reset() { m_start = std::chrono::high_resolution_clock::now(); }
        float Elapsed() { return std::chrono::duration<float, T::period>(std::chrono::high_resolution_clock::now() - m_start).count(); }
    };

    /**
     * \brief Scope-based timer (i.e. begin timer on  creation and displays time on deletion)
     */
    template<chrono_duration T>
    class ScopedTimer
    {
    public:
        ScopedTimer(std::string name) : m_name(std::move(name)) {}
        ~ScopedTimer()
        {
            float time = m_timer.Elapsed();
            std::cout << m_name << " - " << time << "ms\n";
        }
    private:
        Timer<T> m_timer;
        std::string m_name;
    };

    /**
     * \brief Scoped timer to measure all the GPU activity in a scope. To use it, call Begin() when you want to start
     * measure it, then Elapsed when you want to get the elapsed GPU time between the two calls.
     */
    class GPUTimer
    {
    private:
        uint32_t m_query {0};
        int64_t m_timer {0};
    public:
        GPUTimer() { glGenQueries(1, &m_query); };

        /**
         * \brief Creates an OpenGL query to measure time
         */
        void Begin() const
        {
            glBeginQuery(GL_TIME_ELAPSED, m_query);
        }

        /**
         * \brief Ends the query
         */
        void End() const
        {
            glEndQuery(GL_TIME_ELAPSED);
        }

        /**
         * \brief Force the CPU to wait until the GPU returns the query result.
         * Because of this synchronisation process, be careful when using this class.
         * \return The elapsed GPU time between Begin and End, in nanoseconds
         */
        uint64_t Elapsed()
        {
            glGetQueryObjecti64v(m_query, GL_QUERY_RESULT, &m_timer); // stall the CPU
            return m_timer;
        }

        ~GPUTimer()
        {
            glDeleteQueries(1, &m_query);
        }
    };
}