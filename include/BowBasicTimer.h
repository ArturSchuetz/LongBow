#pragma once
#include "BowPrerequisites.h"

#ifdef _WIN32
#include <wrl.h>
#endif

namespace Bow
{
	namespace Core
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
			float GetTotal();

			// Duration in seconds between the previous two calls to Update().
			float GetDelta();

		private:
			#ifdef _WIN32
			LARGE_INTEGER m_frequency;
			LARGE_INTEGER m_currentTime;
			LARGE_INTEGER m_startTime;
			LARGE_INTEGER m_lastTime;
			#endif

			float m_total;
			float m_delta;
		};
	}
}