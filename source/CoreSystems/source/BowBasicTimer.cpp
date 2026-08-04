#include "CoreSystems/BowBasicTimer.h"

#include <CoreSystems/BowLogger.h>

#include <chrono>

namespace bow
{

BasicTimer::BasicTimer()
{
    FN("BasicTimer::BasicTimer");

    Reset();
}

void BasicTimer::Reset()
{
    FN("BasicTimer::Reset");

    Update();
    m_startTime = m_currentTime;
    m_total = 0.0f;
    m_delta = 1.0f / 60.0f;
}

void BasicTimer::Update()
{
    FN("BasicTimer::Update");

    static const float toSeconds = 1.0 / 1000000000.0;
    m_currentTime = std::chrono::system_clock::now();

    m_total = std::chrono::duration_cast<std::chrono::nanoseconds>(m_currentTime - m_startTime).count() * toSeconds;

    if (m_lastTime == m_startTime)
    {
        // If the timer was just reset, report a time delta equivalent to 60Hz
        // frame time.
        m_delta = 1.0f / 60.0f;
    }
    else
    {
        m_delta = std::chrono::duration_cast<std::chrono::nanoseconds>(m_currentTime - m_lastTime).count() * toSeconds;
    }

    m_lastTime = m_currentTime;
}

float BasicTimer::GetTotal()
{
    FN("BasicTimer::GetTotal");

    return m_total;
}

float BasicTimer::GetDelta()
{
    FN("BasicTimer::GetDelta");

    return m_delta;
}
} // namespace bow
