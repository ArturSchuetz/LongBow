#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector2.h"

namespace Bow {
	namespace Core {

		template<typename T>
		class Matrix2x2
		{
		public:
			union
			{
				struct
				{
					T _11, _12;
					T _21, _22;
				};
				T m[2][2];
				T a[4];
			};
			
			Matrix2x2(void)
			{
				SetIdentity();
			}

			Matrix2x2(T __11, T __12, T __21, T __22)
			{
				_11 = __11; _12 = __12; 
				_21 = __21;	_22 = __22;
			}

			inline void SetIdentity(void)
			{
				memset(this, 0, sizeof(Matrix2x2<T>));
				_11 = _22 = 1;
			}

			inline void Set(T __11, T __12, T __21, T __22)
			{
				_11 = __11; _12 = __12;
				_21 = __21;	_22 = __22;
			}

			inline T Determinant(void) const
			{
				return _11 * _22 - _12 * _21;
			}

			inline Matrix2x2 Adjugate(void) const
			{
				return Matrix2x2(_22, -_12, -_21, _11);
			}

			inline Matrix2x2 Inverse(void) const
			{
				return Adjugate() * (1.0 / Determinant());
			}

			inline void Transpose(void)
			{
				Set(_11, _21, _12, _22);
			}

			inline void Rotate(T a)
			{
				T Cos = cos(a);
				T Sin = sin(a);
				return (*this) *= Matrix2x2(Cos, -Sin, Sin, Cos);
			}

			inline void ScaleX(T a)
			{
				return (*this) *= Matrix2x2(a, 0, 0, 1);
			}

			inline void ScaleY(T a)
			{
				return (*this) *= Matrix2x2(1, 0, 0, a);
			}

			inline void Scale(T a)
			{
				return (*this) *= Matrix2x2(a, 0, 0, a);
			}

			template <typename C>
			inline Matrix2x2 operator * (const Matrix2x2<C>& other) const
			{
				return Matrix2x2(
					_11 * other._11 + _12 * other._21,
					_11 * other._12 + _12 * other._22,
					_21 * other._11 + _22 * other._21,
					_21 * other._12 + _22 * other._22
					);
			}

			template <typename C>
			inline Vector2<T> operator * (const Vector2<C>& vector) const
			{
				return Vector2<T>(
					_11 * vector.x + _12 * vector.y,
					_21 * vector.x + _22 * vector.y
					);
			}

			template <typename C>
			inline Matrix2x2 operator * (const C& scalar) const
			{
				return Matrix2x2(
					_11 * scalar, _12 * scalar,
					_21 * scalar, _22 * scalar
					);
			}

			template <typename C>
			inline Matrix2x2 operator / (const C& scalar) const
			{
				return Matrix2x2(
					_11 / scalar, _12 / scalar,
					_21 / scalar, _22 / scalar
					);
			}

			template <typename C>
			inline void operator *= (const Matrix2x2<C>& other)
			{
				Set(
					_11 * other._11 + _12 * other._21,
					_11 * other._12 + _12 * other._22,
					_21 * other._11 + _22 * other._21,
					_21 * other._12 + _22 * other._22
					);
			}
			
			template <typename C>
			inline void operator *= (const C& scalar)
			{
				Set(
					_11 * scalar, _12 * scalar,
					_21 * scalar, _22 * scalar
					);
			}

			template <typename C>
			inline void operator /= (const C& scalar)
			{
				Set(
					_11 / scalar, _12 / scalar,
					_21 / scalar, _22 / scalar
					);
			}

			template <typename C>
			inline bool operator == (const Matrix2x2<C>& other) const
			{
				return _11 == other._11 && _12 == other._12 && _21 == other._21 && _22 == other._22;
			}

			template <typename C>
			inline bool operator != (const Matrix2x2<C>& other) const
			{
				return _11 != other._11 || _12 != other._12 || _21 != other._21 || _22 != other._22;
			}

			template <typename C>
			inline operator Matrix2x2<C>()
			{
				return Matrix2x2<C>((C)_11, (C)_12, (C)_21, (C)_22);
			}
		};
		/*----------------------------------------------------------------*/

		class FloatMatrix2x2 : public Matrix2x2<float>
		{
		public:
			FloatMatrix2x2()
			{
				_11 = _22 = 1.0f;
				_12 = _21 = 0.0f;
			}

			FloatMatrix2x2(float __11, float __12, float __21, float __22)
			{
				_11 = __11; _12 = __12;
				_21 = __21;	_22 = __22;
			}

			FloatVector2 operator * (const FloatVector2& vector) const
			{
				return FloatVector2(
					_11 * vector.x + _12 * vector.y,
					_21 * vector.x + _22 * vector.y
					);
			}
		};
		/*----------------------------------------------------------------*/
	}
}