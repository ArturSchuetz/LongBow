#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <math.h>

namespace Bow {
	namespace Core {

		template <typename T> class Vector4
		{
		public:
			union
			{
				struct
				{
					T x, y, z, w;	// Koordinaten
				};
				T a[4];
			};

			Vector4()
			{
				x = y = z = 0;
				w = 1;
			}

			Vector4(T _x, T _y, T _z, T _w = 1)
			{
				x = _x, y = _y, z = _z, w = _w;
			}

			Vector4(const T* _a)
			{
				x = _a[0];
				y = _a[1];
				z = _a[2];
				w = _a[3];
			}

			inline void Set(T _x, T _y, T _z, T _w = 1)
			{
				x = _x, y = _y, z = _z, w = _w;
			}

			inline T LengthSquared() const
			{
				return x * x + y * y + z * z;
			}

			inline T Length() const
			{
				return sqrt(LengthSquared());
			}

			inline T Angle(const Vector4& other)
			{
				return acos((DotP(other)) / (Length() * other.Length()));
			}

			inline T DotP(const Vector4& other) const
			{
				return x * other.x + y * other.y + z * other.z;
			}

			inline Vector4 CrossP(const Vector4& other) 
			{
				x = y * other.z - z * other.y;
				y = z * other.x - x * other.z;
				z = x * other.y - y * other.x;
				w = 1.0f;
			}

			inline void Normalize()
			{
				(*this) /= Length();
			}

			inline void Negate() const
			{
				x = -x;
				y = -y;
				z = -z;
			}

			void operator += (const Vector4& other)
			{
				x += other.x;
				y += other.y;
				z += other.z;
			}

			Vector4 operator + (const Vector4& other) const
			{
				return Vector4(x + other.x, y + other.y, z + other.z);
			}

			void operator -= (const Vector4& other)
			{
				x -= other.x;
				y -= other.y;
				z -= other.z;
			}

			Vector4 operator - (const Vector4& other) const
			{
				return Vector4(x - other.x, y - other.y, z - other.z);
			}

			void operator *= (T other)
			{
				x *= other;
				y *= other;
				z *= other;
			}

			Vector4 operator * (T other) const
			{
				return Vector4(x * other, y * other, z * other);
			}

			void operator /= (T other)
			{
				x /= other;
				y /= other;
				z /= other;
			}

			Vector4 operator / (T other) const
			{
				return Vector4(x / other, y / other, z / other);
			}

			bool operator == (const Vector4& other) const
			{
				return x == other.x && y == other.y && z == other.z && w == other.w;
			}

			bool operator != (const Vector4& other) const
			{
				return x != other.x || y != other.y || z != other.z || w != other.w;
			}

			template <typename C>
			inline operator Vector4<C>()
			{
				return Vector4<C>((C)x, (C)y, (C)z, (C)w);
			}
		};
		/*----------------------------------------------------------------*/
	}
}