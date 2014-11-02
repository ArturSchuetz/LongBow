#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"

namespace Bow {
	namespace Core {

		template <typename T> class Ray
		{
		public:
			struct
			{
				Vector3<T> Origin, Direction;	// Koordinaten
			};

			Ray()
			{
				Origin = Direction = Vector3<T>();
			}

			Ray(Vector3<T> _origin, Vector3<T> _direction)
			{
				Origin = _origin, Direction = _direction;
			}

			inline void Set(Vector3<T> _origin, Vector3<T> _direction)
			{
				Origin = _origin, Direction = _direction;
			}
		};
		/*----------------------------------------------------------------*/
	}
}