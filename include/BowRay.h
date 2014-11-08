#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"

namespace Bow {
	namespace Core {

		template <typename T> class Ray
		{
		public:
			Vector3<T> Origin, Direction;	// Koordinaten

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

			template <typename C>
			inline operator Ray<C>()
			{
				return Ray<C>((Vector3<C>)Origin, (Vector3<C>)Direction);
			}
		};
		/*----------------------------------------------------------------*/
	}
}