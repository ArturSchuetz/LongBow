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
				x = 0;
				y = 0;
				y = 0;
				w = 1;
			}

			Vector4(T _x, T _y, T _z, T _w = 1)
			{
				x = _x, y = _y, z = _z, w = _w;
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

			inline Vector4 Normalized() const
			{
				return (*this) / Length();
			}

			inline Vector4 Negated() const
			{
				return Vector4(-x, -y, -z);
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
		};
		/*----------------------------------------------------------------*/

		class Vector4LD : public Vector4<long double>
		{
		public:
			Vector4LD()
			{
				x = 0.0;
				y = 0.0;
				y = 0.0;
				w = 1.0;
			}

			Vector4LD(long double _x, long double _y, long double _z, long double _w = 1.0f)
			{
				x = _x, y = _y, z = _z, w = _w;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector4D : public Vector4<double>
		{
		public:
			Vector4D()
			{
				x = 0.0;
				y = 0.0;
				z = 0.0;
				w = 1.0;
			}

			Vector4D(double _x, double _y, double _z, double _w = 1.0f)
			{
				x = _x, y = _y, z = _z, w = _w;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector4F : public Vector4<float>
		{
		public:
			Vector4F()
			{
				x = 0.0;
				y = 0.0;
				z = 0.0;
				w = 1.0;
			}

			Vector4F(float _x, float _y, float _z, float _w = 1.0f)
			{
				x = _x, y = _y, z = _z, w = _w;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector4I : public Vector4<int>
		{
		public:
			Vector4I()
			{
				x = 0;
				y = 0;
				z = 0;
				w = 1;
			}

			Vector4I(int _x, int _y, int _z, int _w = 1)
			{
				x = _x, y = _y, z = _z, w = _w;
			}
		};
		/*----------------------------------------------------------------*/
	}
}