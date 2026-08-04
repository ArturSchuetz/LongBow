#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Math/BowVector3.h"

namespace bow
{

template <typename T> class Triangle
{
  public:
    struct
    {
        Vector3<T> p0, p1, p2;
    };

    Triangle()
    {
        FN("Triangle::Triangle");

        Set(Vector3<T>((T)0, (T)0, (T)0), Vector3<T>((T)0, (T)0, (T)0), Vector3<T>((T)0, (T)0, (T)0));
    }

    Triangle(const Vector3<T> point1, const Vector3<T> point2, const Vector3<T> point3)
    {
        FN("Triangle::Triangle");

        Set(point1, point2, point3);
    }

    inline void Set(const Vector3<T> point1, const Vector3<T> point2, const Vector3<T> point3)
    {
        FN("Triangle::Set");

        p0 = point1;
        p1 = point2;
        p2 = point3;
    }
};
/*----------------------------------------------------------------*/
} // namespace bow
