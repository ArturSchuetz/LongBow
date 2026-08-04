#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Math/BowTriangle.h"
#include "CoreSystems/Math/BowVector3.h"

namespace bow
{

template <typename T> class Ray
{
  public:
    Vector3<T> origin, direction; // Koordinaten

    Ray()
    {
        FN("Ray::Ray");

        origin = direction = Vector3<T>();
    }

    template <typename C> Ray(const Vector3<C> &_origin, const Vector3<C> &_direction)
    {
        FN("Ray::Ray");

        origin = _origin, direction = _direction;
    }

    template <typename C> inline void Set(const Vector3<C> &_origin, const Vector3<C> &_direction)
    {
        FN("Ray::Set");

        origin = _origin, direction = _direction;
    }

    template <typename C> inline void Transform(const Matrix3D<C> &_m)
    {
        FN("Ray::Transform");

        Matrix3D<T> mInv;
        Matrix3D<T> m = _m;

        // inverse translation -- match up zero points
        origin.x -= m._41;
        origin.y -= m._42;
        origin.z -= m._43;

        // delete translation in the matrix. we don't want to have a translation
        // applied to our ray's direction!
        m._41 = 0.0f;
        m._42 = 0.0f;
        m._43 = 0.0f;

        // invert matrix
        mInv = m.Inverse();

        // apply inverse matrix
        origin = mInv * origin;
        direction = mInv * direction;
    }

    template <typename C> inline bool Intersects(const Triangle<C> &triangle, bool bCull, T *t, T fL = -1.0f) const
    {
        FN("Ray::Intersects");

        return Intersects(triangle.p0, triangle.p1, triangle.p2, bCull, t, fL);
    }

    // Intersection With Triangles--uses algorithm from Moller and Trumbore
    // aligns ray with x axis
    // normalizes tri, translates it to origin, and aligns it with yz plane
    // if given a pointer for t, places the distance from ray origin to
    // collision point inside it.
    template <typename C> inline bool Intersects(const Vector3<C> &vc0, const Vector3<C> &vc1, const Vector3<C> &vc2, bool bCull, T *t, T fL = -1.0) const
    {
        FN("Ray::Intersects");

        Vector3<T> pvec, tvec, qvec;

        Vector3<T> edge1 = vc1 - vc0;
        Vector3<T> edge2 = vc2 - vc0;

        pvec = direction.CrossP(edge2);

        // if cloase to 0, ray is parallel
        T det = edge1 * pvec;

        // to account for poor T precision
        // use epsilon value of 0.0001 instead
        // of comparing to zero exactly
        if (bCull && det < 0.00001f)
        {
            return false;
        }
        else if (det < 0.00001f && det > -0.00001f)
        {
            return false;
        }

        // distance to plane, <0 means ray is behind the plane
        tvec = origin - vc0;
        T u = tvec * pvec;
        if (u < 0.0f || u > det)
        {
            return false;
        }

        qvec = tvec.CrossP(edge1);
        T v = direction * qvec;
        if (v < 0.0f || u + v > det)
        {
            return false;
        }

        // at this point, we know that an infinite ray will collide with the
        // triangle. calculate distance to collision
        T fLength;
        fLength = edge2 * qvec;
        T fInvDet = 1.0f / det;
        fLength *= fInvDet;

        // if we've given a max length, check to make sure our collision point
        // isn't beyond it
        if (fL >= 0 && fLength > fL)
        {
            return false;
        }
        if (t)
        {
            *t = fLength;
        }

        return true;
    }

    // Intersection with Planes
    template <typename C> inline bool Intersects(const Plane<C> &plane, bool bCull, T *t, Vector3<C> *vcHit, T fL = -1)
    {
        FN("Ray::Intersects");

        T Vd = plane.normal * direction;

        // ray parallel to the plane
        if (abs(Vd) < 0.00001f)
        {
            return false;
        }

        // plane normal points away from the ray's direction => intersection
        // with the back face if ome exists
        if (bCull && Vd > 0.0f)
        {
            return false;
        }

        T Vo = -((plane.normal * origin) + plane.distance);
        T _t = Vo / Vd;

        // intersection before ray origin
        if (_t < 0.0f || (fL >= 0 && _t > fL))
        {
            return false;
        }

        if (vcHit)
        {
            (*vcHit) = origin + (direction * _t);
        }

        if (t)
        {
            (*t) = _t;
        }

        return true;
    }

    template <typename C> inline operator Ray<C>()
    {
        FN("Ray::Ray");

        return Ray<C>((Vector3<C>)origin, (Vector3<C>)direction);
    }
};
/*----------------------------------------------------------------*/
} // namespace bow
