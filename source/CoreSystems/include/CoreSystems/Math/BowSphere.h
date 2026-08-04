#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Math/BowVector3.h"

namespace bow
{

template <typename T> class Sphere
{
  public:
    Vector3<T> Center;
    T Radius;

    Sphere()
    {
        FN("Sphere::Sphere");

        Center = Vector3<T>();
        Radius = T();
    }

    Sphere(const Vector3<T> center, T radius)
    {
        FN("Sphere::Sphere");

        Center = center;
        Radius = radius;
    }

    inline void Set(Vector3<T> center, T radius)
    {
        FN("Sphere::Set");

        Center = center;
        Radius = radius;
    }
};
/*----------------------------------------------------------------*/
} // namespace bow
