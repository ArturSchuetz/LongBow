#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector2.h"

namespace Bow {
	namespace Core {

		template<typename T>
		class Matrix2D
		{
		public:
			union
			{
				struct
				{
					T _11, _12, _13;
					T _21, _22, _23;
					T _31, _32, _33;
				};
				T m[3][3];
				T a[9];
			};
			
			Matrix2D(void)
			{
				SetIdentity();
			}

			Matrix2D(T __11, T __12, T __21, T __22)
			{
				memset(this, 0, sizeof(Matrix2D<T>));
				_11 = __11; _12 = __12; 
				_21 = __21;	_22 = __22;
				_33 = 1;
			}

			Matrix2D(T __11, T __12, T __13, T __21, T __22, T __23, T __31, T __32, T __33)
			{
				_11 = __11; _12 = __12; _13 = __13;
				_21 = __21; _22 = __22; _23 = __23;
				_31 = __31; _32 = __32; _33 = __33;
			}

			inline void SetIdentity(void)
			{
				memset(this, 0, sizeof(Matrix2D<T>));
				_11 = _22 = _33 = 1;
			}

			inline void Set(T __11, T __12, T __21, T __22)
			{
				_11 = __11; _12 = __12;
				_21 = __21;	_22 = __22;
			}

			inline void Set(T __11, T __12, T __13, T __21, T __22, T __23, T __31, T __32, T __33)
			{
				_11 = __11; _12 = __12; _13 = __13;
				_21 = __21; _22 = __22; _23 = __23;
				_31 = __31; _32 = __32; _33 = __33;
			}

			inline T Determinant(void) const
			{
				return _11 * _22 - _12 * _21;
			}

			inline Matrix2D Adjugate(void) const
			{
				return Matrix2D(_22, -_12, -_21, _11);
			}

			inline Matrix2D Inverse(void) const
			{
				// http://negativeprobability.blogspot.de/2011/11/affine-transformations-and-their.html
				Matrix2D inv = Adjugate() * (1.0 / Determinant());
				Vector2<T> Translation = inv * Vector2<T>(_13, _23);
				inv._13 = -Translation.x;
				inv._23 = -Translation.y;
				return inv;
			}

			inline void Transpose(void)
			{
				Set(_11, _21, _31, _12, _22, _32, _13, _32, _33);
			}

			template <typename C>
			inline void Translate(const Vector2<C>& vector)
			{
				(*this) *= Matrix2D(	
					1, 0, vector.x,
					0, 1, vector.y,
					0, 0, 1
					);
			}

			template <typename C>
			inline void Rotate(C a)
			{
				T Cos = (T)cos(a);
				T Sin = (T)sin(a);
				return (*this) *= Matrix2D(Cos, -Sin, Sin, Cos);
			}

			template <typename C>
			inline void Scale(const Vector2<C>& vector)
			{
				(*this) *= Matrix2D(vector.x, 0, 0, vector.y);
			}

			template <typename C>
			inline void Scale(C a)
			{
				(*this) *= Matrix2D(a, 0, 0, a);
			}

			template <typename C>
			inline Matrix2D operator * (const Matrix2D<C>& other) const
			{
				return Matrix2D(
					_11 * other._11 + _12 * other._21 + _13 * other._31,
					_11 * other._12 + _12 * other._22 + _13 * other._32,
					_11 * other._13 + _12 * other._23 + _13 * other._33,

					_21 * other._11 + _22 * other._21 + _23 * other._31,
					_21 * other._12 + _22 * other._22 + _23 * other._32,
					_21 * other._13 + _22 * other._23 + _23 * other._33,


					_31 * other._11 + _32 * other._21 + _33 * other._31,
					_31 * other._12 + _32 * other._22 + _33 * other._32,
					_31 * other._13 + _32 * other._23 + _33 * other._33
					);
			}

			template <typename C>
			inline Vector2<T> operator * (const Vector2<C>& vector) const
			{
				return Vector2<T>(
					_11 * vector.x + _12 * vector.y + _13,
					_21 * vector.x + _22 * vector.y + _23
				);
			}

			template <typename C>
			inline Vector3<T> operator * (const Vector3<C>& vector) const
			{
				return Vector3<T>(
					_11 * vector.x + _12 * vector.y + _13 * vector.z,
					_21 * vector.x + _22 * vector.y + _23 * vector.z,
					_31 * vector.x + _32 * vector.y + _33 * vector.z
					);
			}

			template <typename C>
			inline Matrix2D operator * (const C& scalar) const
			{
				return Matrix2D(
					_11 * scalar, _12 * scalar, _13 * scalar,
					_21 * scalar, _22 * scalar, _23 * scalar,
					_31 * scalar, _32 * scalar, _33 * scalar
					);
			}

			template <typename C>
			inline Matrix2D operator / (const C& scalar) const
			{
				return Matrix2D(
					_11 / scalar, _12 / scalar, _13 / scalar,
					_21 / scalar, _22 / scalar, _23 / scalar,
					_31 / scalar, _32 / scalar, _33 / scalar
					);
			}

			template <typename C>
			inline void operator *= (const Matrix2D<C>& other)
			{
				Set(
					_11 * other._11 + _12 * other._21 + _13 * other._31,
					_11 * other._12 + _12 * other._22 + _13 * other._32,
					_11 * other._13 + _12 * other._23 + _13 * other._33,

					_21 * other._11 + _22 * other._21 + _23 * other._31,
					_21 * other._12 + _22 * other._22 + _23 * other._32,
					_21 * other._13 + _22 * other._23 + _23 * other._33,


					_31 * other._11 + _32 * other._21 + _33 * other._31,
					_31 * other._12 + _32 * other._22 + _33 * other._32,
					_31 * other._13 + _32 * other._23 + _33 * other._33
					);
			}
			
			template <typename C>
			inline void operator *= (const C& scalar)
			{
				Set(
					_11 * scalar, _12 * scalar, _13 * scalar,
					_21 * scalar, _22 * scalar, _23 * scalar,
					_31 * scalar, _32 * scalar, _33 * scalar
					);
			}

			template <typename C>
			inline void operator /= (const C& scalar)
			{
				Set(
					_11 / scalar, _12 / scalar, _13 / scalar,
					_21 / scalar, _22 / scalar, _23 / scalar,
					_31 / scalar, _32 / scalar, _33 / scalar
					);
			}

			template <typename C>
			inline bool operator == (const Matrix2D<C>& other) const
			{
				return _11 == other._11 && _12 == other._12&& _13 == other._13 && _21 == other._21 && _22 == other._22 && _23 == other._23 && _31 == other._31 && _32 == other._32 && _33 == other._33;
			}

			template <typename C>
			inline bool operator != (const Matrix2D<C>& other) const
			{
				return _11 != other._11 || _12 != other._12|| _13 != other._13 || _21 != other._21 || _22 != other._22 || _23 != other._23 || _31 != other._31 || _32 != other._32 || _33 != other._33;
			}

			template <typename C>
			inline operator Matrix2D<C>()
			{
				return Matrix2D<C>((C)_11, (C)_12, (C)_13, (C)_21, (C)_22, (C)_23, (C)_31, (C)_32, (C)_33);
			}
		};
		/*----------------------------------------------------------------*/

		class FloatMatrix2D : public Matrix2D<float>
		{
		public:
			FloatMatrix2D()
			{
				_11 = _22 = 1.0f;
				_12 = _21 = 0.0f;
			}

			FloatMatrix2D(float __11, float __12, float __21, float __22)
			{
				_11 = __11; _12 = __12;
				_21 = __21;	_22 = __22;
			}

			inline FloatVector2 operator * (const FloatVector2& vector) const
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