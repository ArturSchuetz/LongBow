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
				x = 0;
				y = 0;
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

			inline Vector2 Normalized() const
			{
				return (*this) / Length();
			}

			inline Vector2 Negated() const
			{
				return Vector2(-x, -y);
			}

			void operator += (const Vector2& other)
			{
				x += other.x;
				y += other.y;
			}

			Vector2 operator + (const Vector2& other) const
			{
				return Vector2(x + other.x, y + other.y);
			}

			void operator -= (const Vector2& other)
			{
				x -= other.x;
				y -= other.y;
			}

			Vector2 operator - (const Vector2& other) const
			{
				return Vector2(x - other.x, y - other.y);
			}

			void operator *= (T other)
			{
				x *= other;
				y *= other;
			}

			Vector2 operator * (T other) const
			{
				return Vector2(x * other, y * other);
			}

			void operator /= (T other)
			{
				x /= other;
				y /= other;
			}

			Vector2 operator / (T other) const
			{
				return Vector2(x / other, y / other);
			}

			bool operator == (const Vector2& other) const
			{
				return x == other.x && y == other.y;
			}

			bool operator != (const Vector2& other) const
			{
				return x != other.x || y != other.y;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector2LD : public Vector2<long double>
		{
		public:
			Vector2LD()
			{
				x = 0.0;
				y = 0.0;
			}

			Vector2LD(long double _x, long double _y)
			{
				x = _x, y = _y;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector2D : public Vector2<double>
		{
		public:
			Vector2D()
			{
				x = 0.0;
				y = 0.0;
			}

			Vector2D(double _x, double _y)
			{
				x = _x, y = _y;
			}
		};
		/*----------------------------------------------------------------*/

		class Vector2F : public Vector2<float>
		{
		public:
			Vector2F()
			{
				x = 0.0;
				y = 0.0;
			}

			Vector2F(float _x, float _y)
			{
				x = _x, y = _y;
			}

		};
		/*----------------------------------------------------------------*/

		class Vector2I : public Vector2<int>
		{
		public:
			Vector2I()
			{
				x = 0;
				y = 0;
			}

			Vector2I(int _x, int _y)
			{
				x = _x, y = _y;
			}
		};
		/*----------------------------------------------------------------*/
	}
}