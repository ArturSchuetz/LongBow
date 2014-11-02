#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <math.h>

namespace Bow {
	namespace Core {

		template <typename T> class Vector3
		{
		public:
			union
			{
				struct
				{
					T x, y, z;	// Koordinaten
				};
				T a[3];
			};

			Vector3()
			{
				x = y = z = 0;
			}

			Vector3(T _x, T _y, T _z)
			{
				x = _x, y = _y, z = _z;
			}

			Vector3(const T* _a)
			{
				x = _a[0];
				y = _a[1];
				z = _a[2];
			}

			inline void Set(T _x, T _y, T _z)
			{
				x = _x, y = _y, z = _z;
			}

			inline T LengthSquared() const
			{
				return this->x * x + this->y * y + this->z * z;
			}

			inline T Length() const
			{
				return sqrt(LengthSquared());
			}

			inline T Angle(const Vector3& other)
			{
				return acos((DotP(other)) / (Length() * other.Length()));
			}

			inline T DotP(const Vector3& other) const
			{
				return x * other.x + y * other.y + z * other.z;
			}

			inline Vector3 CrossP(const Vector3& other) const
			{
				return Vector3(	y * other.z - z * other.y,	// x
								z * other.x - x * other.z,	// y
								x * other.y - y * other.x); // z
			}

			inline void Normalize()
			{
				return (*this) /= Length();
			}

			inline void Negate()
			{
				x = -x;
				y = -y;
				z = -z;
			}

			inline Vector3 Negative() const
			{
				return Vector3(-x,-y,-z);
			}

			void operator += (const Vector3& other)
			{
				x += other.x;
				y += other.y;
				z += other.z;
			}

			Vector3 operator + (const Vector3& other) const
			{
				return Vector3(x + other.x, y + other.y, z + other.z);
			}

			void operator -= (const Vector3& other)
			{
				x -= other.x;
				y -= other.y;
				z -= other.z;
			}

			Vector3 operator - (const Vector3& other) const
			{
				return Vector3(x - other.x, y - other.y, z - other.z);
			}

			void operator *= (T other)
			{
				x *= other;
				y *= other;
				z *= other;
			}

			Vector3 operator * (T other) const
			{
				return Vector3(x * other, y * other, z * other);
			}

			void operator /= (T other)
			{
				x /= other;
				y /= other;
				z /= other;
			}

			Vector3 operator / (T other) const
			{
				return Vector3(x / other, y / other, z / other);
			}

			bool operator == (const Vector3& other) const
			{
				return x == other.x && y == other.y && z == other.z;
			}

			bool operator != (const Vector3& other) const
			{
				return x != other.x || y != other.y || z != other.z;
			}

			template <typename C>
			inline operator Vector3<C>()
			{
				return Vector3<C>((C)x, (C)y, (C)z);
			}
		};
		/*----------------------------------------------------------------*/
	}
}