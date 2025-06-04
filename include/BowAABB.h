/**
 * @file BowAABB.h
 * @brief Declarations for BowAABB.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"

namespace bow {

	template<typename T>
	class AABB
	{
	public:
		Vector3<T> min, max;	//extreme points
		Vector3<T> center;		//center point

		AABB(void) 
		{
			Set(Vector3<T>::Zero(), Vector3<T>::Zero());
		}

		AABB(Vector3<T> _min, Vector3<T> _max) 
		{
			Set(_min, _max);
		}

		inline void Set(Vector3<T> _min, Vector3<T> _max)
		{
			min = _min;
			max = _max;

			center = Vector3<T>(0, 0, 0);
			center += Vector3<T>(min.x, min.y, min.z);
			center += Vector3<T>(min.x, min.y, max.z);
			center += Vector3<T>(min.x, max.y, min.z);
			center += Vector3<T>(min.x, max.y, max.z);
			center += Vector3<T>(max.x, min.y, min.z);
			center += Vector3<T>(max.x, min.y, max.z);
			center += Vector3<T>(max.x, max.y, min.z);
			center += Vector3<T>(max.x, max.y, max.z);
			center /= 8;
		}

	};

	/*----------------------------------------------------------------*/

	template<typename C>
	inline std::ostream& operator << (std::ostream& str, const AABB<C>& mboundingbox)
	{
		str << "Center: " << mboundingbox.center << ", Min: " << mboundingbox.min << ", Max: " << mboundingbox.max;
		return str;
	}

	/*-----------------------------------------------------------------------------------------*/
	MeshAttribute CreateDebugMesh(const AABB<float>& aabb, const std::string& vertexAttributeName);
}
