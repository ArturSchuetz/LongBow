#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <math.h>
#include "BowLogger.h"

#include "BowVector3.h"

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

			Vector4(const Vector3<T> &other,T _w = 1)
			{
				x = other.x, y = other.y, z = other.z, w = _w;
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

			inline T Length() const
			{
				return sqrt(LengthSquared());
			}

			inline T LengthSquared() const
			{
				return x * x + y * y + z * z;
			}

			inline T Angle(const Vector4& other)
			{
				return acos((DotP(other)) / (Length() * other.Length()));
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

			inline Vector4 operator - () const
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

			// write a value
			inline T& operator [](int index)
			{
				LOG_ASSERT(index >= 0 && index < 4, "Out of bounds of Vector4");

				return a[index];
			}

			// read a value
			inline const T& operator [](int index) const
			{
				LOG_ASSERT(index >= 0 && index < 4, "Out of bounds of Vector4");

				return a[index];
			}

			template <typename C>
			inline operator Vector4<C>()
			{
				return Vector4<C>((C)x, (C)y, (C)z, (C)w);
			}
		};
		/*----------------------------------------------------------------*/

		template <typename C, typename T>
		inline Vector4<T> operator *(C s, const Vector4<T>& vector)
		{
			return vector * (T)s;
		}

		// Dot product
		template <typename T> 
		inline T DotP(const Vector4<T>& v1, const Vector4<T>& v2)
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
		}

		// Cross product calculation for a 3D Vector
		template <typename T> 
		inline Vector4<T> CrossP(const Vector4<T>& v1, const Vector4<T>& v2)
		{
			return Vector4<T>(
				v1.y * other.z - v1.z * other.y,
				v1.z * other.x - v1.x * other.z,
				v1.x * other.y - v1.y * other.x,
				1.0f);
		}

		typedef Vector4<float> ColorRGBA;
	}
}