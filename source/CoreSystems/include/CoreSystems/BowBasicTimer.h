#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include <chrono>

namespace bow
{

// Helper class for basic timing.
class CORESYSTEMS_API BasicTimer
{
  public:
    // Initializes internal timer values.
    BasicTimer();

    // Reset the timer to initial values.
    void Reset();

    // Update the timer's internal values.
    void Update();

    // Duration in seconds between the last call to Reset() and the last call to
    // Update().
    float GetTotal();

    // Duration in seconds between the previous two calls to Update().
    float GetDelta();

  private:
    std::chrono::time_point<std::chrono::system_clock> m_currentTime;
    std::chrono::time_point<std::chrono::system_clock> m_startTime;
    std::chrono::time_point<std::chrono::system_clock> m_lastTime;

    float m_total;
    float m_delta;
};

} // namespace bow
