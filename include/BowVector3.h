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
				x = 0;
				y = 0;
				y = 0;
			}

			Vector3(T _x, T _y, T _z)
			{
				x = _x, y = _y, z = _z;
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
				x = y * other.z - z * other.y;
				y = z * other.x - x * other.z;
				z = x * other.y - y * other.x;
			}

			inline Vector3 Normalized() const
			{
				return (*this) / Length();
			}

			inline Vector3 Negated() const
			{
				return Vector3(-x, -y, -z);
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
		};
		/*----------------------------------------------------------------*/

		class Vector3LD : public Vector3<long double>
		{
		public:
			Vector3LD()
			{
				x = 0.0;
				y = 0.0;
				z = 0.0;
			}

			Vector3LD(long double _x, long double _y, long double _z)
			{
				x = _x, y = _y, z = _z;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector3D : public Vector3<double>
		{
		public:
			Vector3D()
			{
				x = 0.0;
				y = 0.0;
				z = 0.0;
			}

			Vector3D(double _x, double _y, double _z)
			{
				x = _x, y = _y, z = _z;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector3F : public Vector3<float>
		{
		public:
			Vector3F()
			{
				x = 0.0;
				y = 0.0;
				z = 0.0;
			}

			Vector3F(float _x, float _y, float _z)
			{
				x = _x, y = _y, z = _z;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector3I : public Vector3<int>
		{
		public:
			Vector3I()
			{
				x = 0;
				y = 0;
				z = 0;
			}

			Vector3I(int _x, int _y, int _z)
			{
				x = _x, y = _y, z = _z;
			}
		};
		/*----------------------------------------------------------------*/
	}
}