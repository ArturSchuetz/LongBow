/**
 * @file BowBasicTimer.h
 * @brief Declarations for BowBasicTimer.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#ifdef _WIN32
#include <wrl.h>
#endif

namespace bow
{
	// Helper class for basic timing.
	class BasicTimer
	{
	public:
		// Initializes internal timer values.
		BasicTimer();

		// Reset the timer to initial values.
		void Reset();

		// Update the timer's internal values.
		void Update();

		// Duration in seconds between the last call to Reset() and the last call to Update().
		double GetTotal();

		// Duration in seconds between the previous two calls to Update().
		double GetDelta();

	private:
		#ifdef _WIN32
		LARGE_INTEGER m_frequency;
		LARGE_INTEGER m_currentTime;
		LARGE_INTEGER m_startTime;
		LARGE_INTEGER m_lastTime;
		#endif

		double m_total;
		double m_delta;
	};
}
