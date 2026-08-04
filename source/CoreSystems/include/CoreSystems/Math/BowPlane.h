#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Math/BowVector3.h"

namespace bow
{

template <typename T> class Plane
{
  public:
    struct
    {
        Vector3<T> point, normal;
        T distance;
    };

    Plane() { FN("Plane::Plane"); }

    template <typename C> inline void Set(Vector3<C> _normal, Vector3<C> _point)
    {
        FN("Plane::Set");

        distance = -(_normal * _point);
        normal = _normal;
        point = _point;
    }

    template <typename C> inline void Set(Vector3<C> _normal, Vector3<C> _point, float _distance)
    {
        FN("Plane::Set");

        distance = _distance;
        normal = _normal;
        point = _point;
    }

    template <typename C> inline void Set(Vector3<C> vec1, Vector3<C> vec2, Vector3<C> vec3)
    {
        FN("Plane::Set");

        Vector3 edge1 = vec2 - vec1;
        Vector3 edge2 = vec3 - vec1;

        normal = edge1.Cross(edge2);
        distance = -(normal * vec1);
        point = vec1;
    }

    template <typename C> inline float Distance(Vector3<C> vcPoint)
    {
        FN("Plane::Distance");

        return abs((normal * vcPoint) - distance);
    }
};
/*----------------------------------------------------------------*/
} // namespace bow
