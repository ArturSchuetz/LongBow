#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"

namespace bow {

	template <typename T> class Triangle
	{
	public:
		struct{
			Vector3<T>	p0, p1, p2;
		};

		Triangle()
		{
			Set(Vector3<T>((T)0, (T)0, (T)0), Vector3<T>((T)0, (T)0, (T)0), Vector3<T>((T)0, (T)0, (T)0));
		}

		Triangle(const Vector3<T> point1, const Vector3<T> point2, const Vector3<T> point3)
		{
			Set(point1, point2, point3);
		}

		inline void Set(const Vector3<T> point1, const Vector3<T> point2, const Vector3<T> point3)
		{
			p0 = point1;
			p1 = point2;
			p2 = point3;
		}
	};
	/*----------------------------------------------------------------*/
}
