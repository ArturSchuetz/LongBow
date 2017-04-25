#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"
#include "BowTriangle.h"

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

			Ray(const Vector3<T> &_origin, const  Vector3<T> &_direction)
			{
				Origin = _origin, Direction = _direction;
			}

			inline void Set(const Vector3<T> &_origin, const Vector3<T> &_direction)
			{
				Origin = _origin, Direction = _direction;
			}

			inline void Transform(const Matrix3D<T> &_m)
			{
				Matrix3D<T> mInv;
				Matrix3D<T> m = _m;

				//inverse translation -- match up zero points
				Origin.x -= m._41;
				Origin.y -= m._42;
				Origin.z -= m._43;

				//delete translation in the matrix. we don't want to have a translation applied to our ray's direction!
				m._41 = 0.0f;
				m._42 = 0.0f;
				m._43 = 0.0f;

				//invert matrix
				mInv = m.Inverse();

				//apply inverse matrix
				Origin = mInv * Origin;
				Direction = mInv * Direction;
			}

			inline bool Intersects(const Triangle<T> &triangle, bool bCull, double *t, double fL = -1.0f) const
			{
				return Intersects(triangle.p0, triangle.p1, triangle.p2, bCull, t, fL);
			}

			//Intersection With Triangles--uses algorithm from Moller and Trumbore
			//aligns ray with x axis
			//normalizes tri, translates it to origin, and aligns it with yz plane
			//if given a pointer for t, places the distance from ray origin to collision point inside it.
			inline bool Intersects(const Vector3<T> &vc0, const Vector3<T> &vc1, const Vector3<T> &vc2, bool bCull, double *t, double fL = -1.0) const
			{
				Vector3<T> pvec, tvec, qvec;

				Vector3<T> edge1 = vc1 - vc0;
				Vector3<T> edge2 = vc2 - vc0;

				pvec = Direction.CrossP(edge2);

				//if cloase to 0, ray is parallel
				double det = edge1 * pvec;

				//to account for poor float precision
				//use epsilon value of 0.0001 instead
				//of comparing to zero exactly
				if (bCull && det < 0.00001f)
				{
					return false;
				}
				else if (det < 0.00001f && det > -0.00001f)
				{
					return false;
				}

				//distance to plane, <0 means ray is behind the plane
				tvec = Origin - vc0;
				double u = tvec * pvec;
				if (u < 0.0f || u > det)
				{
					return false;
				}

				qvec = tvec.CrossP(edge1);
				double v = Direction * qvec;
				if (v < 0.0f || u + v > det)
				{
					return false;
				}

				//at this point, we know that an infinite ray will collide with the triangle. calculate distance to collision
				double fLength;
				fLength = edge2 * qvec;
				double fInvDet = 1.0f / det;
				fLength *= fInvDet;

				//if we've given a max length, check to make sure our collision point isn't beyond it
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

			template <typename C>
			inline operator Ray<C>()
			{
				return Ray<C>((Vector3<C>)Origin, (Vector3<C>)Direction);
			}
		};
		/*----------------------------------------------------------------*/
	}
}