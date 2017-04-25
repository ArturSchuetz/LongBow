#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"

namespace Bow {
	namespace Core {

		template <typename T> class Plane
		{
		public:
			struct
			{
				Vector3<T>	Point, Normal;
				T			Distance;
			};

			Plane()
			{
			}
		};
		/*----------------------------------------------------------------*/
	}
}