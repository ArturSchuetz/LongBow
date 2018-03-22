#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowPlane.h"
#include "BowMatrix4x4.h"

namespace bow {

	enum CullingPosition : char
	{
		Inside = 0,
		Near = 1,
		Far = 2,
		Left = 4,
		Right = 8,
		Top = 16,
		Bottom = 32,
	};

	template <typename T> class Frustum
	{
	public:
		union
		{
			struct
			{
				Plane<T> _near;
				Plane<T> _far;
				Plane<T> _left;
				Plane<T> _right;
				Plane<T> _top;
				Plane<T> _bottom;
			};
			Plane<T> _planes[6];
		};
			
		Frustum()
		{

		}

		Frustum(const Plane<T>& near, const Plane<T>& far, const Plane<T>& left, const Plane<T>& right, const Plane<T>& top, const Plane<T>& bottom)
		{
			Set(near, far, left, right, top, bottom);
		}

		Frustum(const Matrix4x4<T>& modelViewProjection)
		{
			CalculateFrustum(modelViewProjection);
		}

		inline void Set(const Plane<T>& near, const Plane<T>& far, const Plane<T>& left, const Plane<T>& right, const Plane<T>& top, const Plane<T>& bottom)
		{
			_near = near;
			_far = far;
			_left = left;
			_right = right;
			_top = top;
			_bottom = bottom;
		}

		inline Frustum* CalculateFrustum(const Matrix4x4<T>& modelViewProjection)
		{
			// Calculate near plane of frustum.
			_near.Normal.x = (modelViewProjection._41 + modelViewProjection._31);
			_near.Normal.y = (modelViewProjection._42 + modelViewProjection._32);
			_near.Normal.z = (modelViewProjection._43 + modelViewProjection._33);
			_near.Distance = (modelViewProjection._44 + modelViewProjection._34);

			// Calculate far plane of frustum.
			_far.Normal.x = (modelViewProjection._41 - modelViewProjection._31);
			_far.Normal.y = (modelViewProjection._42 - modelViewProjection._32);
			_far.Normal.z = (modelViewProjection._43 - modelViewProjection._33);
			_far.Distance = (modelViewProjection._44 - modelViewProjection._34);

			// Calculate left plane of frustum.
			_left.Normal.x = (modelViewProjection._41 + modelViewProjection._11);
			_left.Normal.y = (modelViewProjection._42 + modelViewProjection._12);
			_left.Normal.z = (modelViewProjection._43 + modelViewProjection._13);
			_left.Distance = (modelViewProjection._44 + modelViewProjection._14);

			// Calculate right plane of frustum.
			_right.Normal.x = (modelViewProjection._41 - modelViewProjection._11);
			_right.Normal.y = (modelViewProjection._42 - modelViewProjection._12);
			_right.Normal.z = (modelViewProjection._43 - modelViewProjection._13);
			_right.Distance = (modelViewProjection._44 - modelViewProjection._14);

			// Calculate top plane of frustum.
			_top.Normal.x = (modelViewProjection._41 - modelViewProjection._21);
			_top.Normal.y = (modelViewProjection._42 - modelViewProjection._22);
			_top.Normal.z = (modelViewProjection._43 - modelViewProjection._23);
			_top.Distance = (modelViewProjection._44 - modelViewProjection._24);

			// Calculate bottom plane of frustum.
			_bottom.Normal.x = (modelViewProjection._41 + modelViewProjection._21);
			_bottom.Normal.y = (modelViewProjection._42 + modelViewProjection._22);
			_bottom.Normal.z = (modelViewProjection._43 + modelViewProjection._23);
			_bottom.Distance = (modelViewProjection._44 + modelViewProjection._24);

			//normalize normals
			for (unsigned int i = 0; i < 6; i++)
			{
				T norm = sqrt(_planes[i].Normal.x * _planes[i].Normal.x + _planes[i].Normal.y * _planes[i].Normal.y + _planes[i].Normal.z * _planes[i].Normal.z);
				if (norm != 0.0)
				{
					_planes[i].Normal.x /= norm;
					_planes[i].Normal.y /= norm;
					_planes[i].Normal.z /= norm;
					_planes[i].Distance /= norm;
				}
				else
				{
					_planes[i].Normal.x = 0.0;
					_planes[i].Normal.y = 0.0;
					_planes[i].Normal.z = 0.0;
					_planes[i].Distance = 0.0;
				}
			}

			return this;
		}

		inline bool IsVisible(const Vector3<double>& position)
		{
			for (unsigned int i = 0; i < 6; i++)
			{
				double f = (position.DotP(_planes[i].Normal)) + _planes[i].Distance;

				if (f < 0.0)
				{
					return false;
				}
			}

			return true;
		}

		inline bool IsTriangleVisible(const Vector3<double>& p0, const Vector3<double>& p1, const Vector3<double>& p2)
		{
			if ((p1 - p0).CrossP(p2 - p0).DotP(_far.Normal) < -0.01f)
				return false;

			char p0_culling = 0;
			for (unsigned int i = 0; i < 6; i++)
			{
				double f = (p0.DotP(_planes[i].Normal)) + _planes[i].Distance;
				if (f < 0.0)
				{
					p0_culling += (char)pow(2, (i));
				}
			}

			char p1_culling = 0;
			for (unsigned int i = 0; i < 6; i++)
			{
				double f = (p1.DotP(_planes[i].Normal)) + _planes[i].Distance;
				if (f < 0.0)
				{
					p1_culling += (char)pow(2, (i));
				}
			}

			char p2_culling = 0;
			for (unsigned int i = 0; i < 6; i++)
			{
				double f = (p2.DotP(_planes[i].Normal)) + _planes[i].Distance;
				if (f < 0.0)
				{
					p2_culling += (char)pow(2, (i));
				}
			}
				
			if (((p0_culling & CullingPosition::Near) != 0	&& (p1_culling & CullingPosition::Near) != 0	&& (p2_culling & CullingPosition::Near) != 0) ||
				((p0_culling & CullingPosition::Far) != 0	&& (p1_culling & CullingPosition::Far) != 0		&& (p2_culling & CullingPosition::Far) != 0) ||
				((p0_culling & CullingPosition::Right) != 0 && (p1_culling & CullingPosition::Right) != 0	&& (p2_culling & CullingPosition::Right) != 0) ||
				((p0_culling & CullingPosition::Left) != 0	&& (p1_culling & CullingPosition::Left) != 0	&& (p2_culling & CullingPosition::Left) != 0) ||
				((p0_culling & CullingPosition::Top) != 0	&& (p1_culling & CullingPosition::Top) != 0		&& (p2_culling & CullingPosition::Top) != 0) ||
				((p0_culling & CullingPosition::Bottom) != 0 && (p1_culling & CullingPosition::Bottom) != 0 && (p2_culling & CullingPosition::Bottom) != 0))
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	};
	/*----------------------------------------------------------------*/
}
