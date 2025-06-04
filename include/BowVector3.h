/**
 * @file BowVector3.h
 * @brief Declarations for BowVector3.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <math.h>
#include "BowLogger.h"

namespace bow {

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

		inline static Vector3 Zero()
		{
			return Vector3(0,0,0);
		}

		inline static Vector3 One()
		{
			return Vector3(1, 1, 1);
		}
			
		inline static Vector3 Lerp(Vector3 vectorA, Vector3 vectorB, T alpha)
		{
			return vectorA * (1 - alpha) + vectorB * alpha;
		}

		inline void Set(T _x, T _y, T _z)
		{
			x = _x, y = _y, z = _z;
		}

		inline T Length() const
		{
			return sqrt(LengthSquared());
		}

		inline T LengthSquared() const
		{
			return this->x * x + this->y * y + this->z * z;
		}

		inline T Angle(const Vector3& other)
		{
			return acos((DotP(*this, other)) / (Length() * other.Length()));
		}

		inline void Normalize()
		{
			return (*this) /= Length();
		}

		inline Vector3 Normalized() const
		{
			return (*this) / Length();
		}

		inline void Negate()
		{
			x = -x;
			y = -y;
			z = -z;
		}

		inline T DotP(const Vector3& other) const
		{
			return x * other.x + y * other.y + z * other.z;
		}

		inline Vector3 CompP(const Vector3& other) const
		{
			return Vector3(x* other.x, y * other.y, z * other.z);
		}

		inline Vector3 CrossP(const Vector3& other) const
		{
			return Vector3<T>(	y * other.z - z * other.y,	// x
								z * other.x - x * other.z,	// y
								x * other.y - y * other.x); // z
		}

		inline Vector3 operator - () const
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

		T operator * (const Vector3 &other) const
		{
			return x * other.x + y * other.y + z * other.z;
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

		// write a value
		inline T& operator [](int index)
		{
			LOG_ASSERT(index >= 0 && index < 3, "Out of bounds of Vector3");

			return a[index];
		}

		// read a value
		inline const T& operator [](int index) const
		{
			LOG_ASSERT(index >= 0 && index < 3, "Out of bounds of Vector3");

			return a[index];
		}

		template <typename C>
		inline operator Vector3<C>() const
		{
			return Vector3<C>((C)x, (C)y, (C)z);
		}

		template <typename C>
		inline operator Vector3<C>()
		{
			return Vector3<C>((C)x, (C)y, (C)z);
		}
	};
	/*----------------------------------------------------------------*/

	//template <typename C, typename T>
	//inline Vector3<T> operator *(C s, const Vector3<T>& vector)
	//{
	//	return vector * (T)s;
	//}

	// Dot product
	template <typename T> 
	inline T DotP(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	// Cross product
	template <typename T> 
	inline Vector3<T> CrossP(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		return Vector3<T>(v1.y * v2.z - v1.z * v2.y,	// x
							v1.z * v2.x - v1.x * v2.z,	// y
							v1.x * v2.y - v1.y * v2.x); // z
	}

	// Cross product
	template <typename T>
	inline Vector3<T> CompP(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		return Vector3<T>(	v1.x * v2.x,	// x
							v1.y * v2.y,	// y
							v1.z * v2.z); // z
	}

	template<typename C>
	inline std::ostream& operator << (std::ostream& str, const Vector3<C>& vec)
	{
		str << '[';
		for (unsigned int row = 0; row < 3; ++row)
		{
			str << vec.a[row];

			if (row != 2)
				str << ", ";
		}
		str << "]";

		return str;
	}

	typedef Vector3<float> ColorRGB;
}
