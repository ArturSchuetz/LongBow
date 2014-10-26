#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <math.h>

namespace Bow {
	namespace Core {

		template <typename T> class Vector2
		{
		public:
			union
			{
				struct
				{
					T x, y;	// Koordinaten
				};
				T a[2];
			};

			Vector2()
			{
				x = y = 0;
			}

			Vector2(T _x, T _y)
			{
				x = _x, y = _y;
			}

			inline void Set(T _x, T _y)
			{
				x = _x, y = _y;
			}

			inline T LengthSquared() const
			{
				return x * x + y * y;
			}

			inline T Length() const
			{
				return sqrt(LengthSquared());
			}

			inline T Angle(const Vector2& other)
			{
				return acos((DotP(other)) / (Length() * other.Length()));
			}

			inline T DotP(const Vector2& other) const
			{
				return x * other.x + y * other.y;
			}

			inline void Normalize()
			{
				(*this) /= Length();
			}

			inline void Negate()
			{
				x = -x;
				y = -y;
			}

			inline void operator += (const Vector2& other)
			{
				x += other.x;
				y += other.y;
			}

			inline Vector2 operator + (const Vector2& other) const
			{
				return Vector2(x + other.x, y + other.y);
			}

			inline void operator -= (const Vector2& other)
			{
				x -= other.x;
				y -= other.y;
			}

			inline Vector2 operator - (const Vector2& other) const
			{
				return Vector2(x - other.x, y - other.y);
			}

			inline void operator *= (T other)
			{
				x *= other;
				y *= other;
			}

			inline Vector2 operator * (T other) const
			{
				return Vector2(x * other, y * other);
			}

			inline void operator /= (T other)
			{
				x /= other;
				y /= other;
			}

			inline Vector2 operator / (T other) const
			{
				return Vector2(x / other, y / other);
			}

			inline bool operator == (const Vector2& other) const
			{
				return x == other.x && y == other.y;
			}

			inline bool operator != (const Vector2& other) const
			{
				return x != other.x || y != other.y;
			}

			template <typename C>
			inline operator Vector2<C>()
			{
				return Vector2<C>((C)x, (C)y);
			}
		};
		/*----------------------------------------------------------------*/

		class FloatVector2 : public Vector2<float>
		{
		public:
			FloatVector2()
			{
				x = 0.0;
				y = 0.0;
			}

			FloatVector2(float _x, float _y)
			{
				x = _x, y = _y;
			}

			template <typename C>
			FloatVector2(const Vector2<C>& other)
			{
				Set(other.x, other.y);
			}
		};
		/*----------------------------------------------------------------*/
	}
}