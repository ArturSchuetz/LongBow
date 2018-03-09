#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector2.h"
#include "BowVector3.h"

namespace bow {

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
			memset(this, 0, sizeof(Matrix2D));
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
			Matrix2D inv = Adjugate() * ((T)1 / Determinant());
			Vector2<T> Translation = inv * Vector2<T>(_13, _23);
			inv._13 = -Translation.x;
			inv._23 = -Translation.y;
			inv._33 = (T)1;
			return inv;
		}

		inline void Transpose(void)
		{
			Set(_11, _21, _31, _12, _22, _32, _13, _32, _33);
		}

		inline void Translate(const Vector2<T>& vector)
		{
			(*this) *= Matrix2D(	
				(T)1, (T)0, (T)vector.x,
				(T)0, (T)1, (T)vector.y,
				(T)0, (T)0, (T)1
				);
		}

		inline void Rotate(T a)
		{
			T Cos = (T)cos(a);
			T Sin = (T)sin(a);
			return (*this) *= Matrix2D(Cos, -Sin, Sin, Cos);
		}

		inline void Scale(const Vector2<T>& vector)
		{
			(*this) *= Matrix2D(vector.x, (T)0, (T)0, vector.y);
		}

		inline void Scale(T a)
		{
			(*this) *= Matrix2D(a, (T)0, (T)0, a);
		}

		inline Matrix2D operator * (const Matrix2D& other) const
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

		inline Vector2<T> operator * (const Vector2<T>& vector) const
		{
			return Vector2<T>(
				_11 * vector.x + _12 * vector.y + _13,
				_21 * vector.x + _22 * vector.y + _23
			);
		}

		inline Vector3<T> operator * (const Vector3<T>& vector) const
		{
			return Vector3<T>(
				_11 * vector.x + _12 * vector.y + _13 * vector.z,
				_21 * vector.x + _22 * vector.y + _23 * vector.z,
				_31 * vector.x + _32 * vector.y + _33 * vector.z
				);
		}

		inline Matrix2D operator * (const T& scalar) const
		{
			return Matrix2D(
				_11 * scalar, _12 * scalar, _13 * scalar,
				_21 * scalar, _22 * scalar, _23 * scalar,
				_31 * scalar, _32 * scalar, _33 * scalar
				);
		}

		inline Matrix2D operator / (const T& scalar) const
		{
			return Matrix2D(
				_11 / scalar, _12 / scalar, _13 / scalar,
				_21 / scalar, _22 / scalar, _23 / scalar,
				_31 / scalar, _32 / scalar, _33 / scalar
				);
		}

		inline void operator *= (const Matrix2D& other)
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
			
		inline void operator *= (const T& scalar)
		{
			Set(
				_11 * scalar, _12 * scalar, _13 * scalar,
				_21 * scalar, _22 * scalar, _23 * scalar,
				_31 * scalar, _32 * scalar, _33 * scalar
				);
		}

		inline void operator /= (const T& scalar)
		{
			Set(
				_11 / scalar, _12 / scalar, _13 / scalar,
				_21 / scalar, _22 / scalar, _23 / scalar,
				_31 / scalar, _32 / scalar, _33 / scalar
				);
		}

		inline bool operator == (const Matrix2D& other) const
		{
			return _11 == other._11 && _12 == other._12&& _13 == other._13 && _21 == other._21 && _22 == other._22 && _23 == other._23 && _31 == other._31 && _32 == other._32 && _33 == other._33;
		}

		inline bool operator != (const Matrix2D& other) const
		{
			return _11 != other._11 || _12 != other._12|| _13 != other._13 || _21 != other._21 || _22 != other._22 || _23 != other._23 || _31 != other._31 || _32 != other._32 || _33 != other._33;
		}

		template<typename C>
		inline operator Matrix2D<C>()
		{
			return Matrix2D<C>((C)_11, (C)_12, (C)_13, (C)_21, (C)_22, (C)_23, (C)_31, (C)_32, (C)_33);
		}
	};
	/*----------------------------------------------------------------*/

	template <typename T> inline Matrix2D<T> operator *(float s, const Matrix2D<T>& matrix)
	{
		return matrix * s;
	}
}
