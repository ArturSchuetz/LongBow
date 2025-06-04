/**
 * @file BowPlane.h
 * @brief Declarations for BowPlane.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"

namespace bow {

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
