#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"
#include "BowVector4.h"

namespace bow {

	template<typename T>
	class Matrix4x4
	{
	public:
		union
		{
			struct
			{
				T _11, _12, _13, _14;
				T _21, _22, _23, _24;
				T _31, _32, _33, _34;
				T _41, _42, _43, _44;
			};
			T m[4][4];
			T a[16];
		};

		Matrix4x4(void)
		{
			SetIdentity();
		}

		Matrix4x4(T __11, T __12, T __13, T __14, T __21, T __22, T __23, T __24, T __31, T __32, T __33, T __34, T __41, T __42, T __43, T __44)
		{
			Set(__11, __12, __13, __14, __21, __22, __23, __24, __31, __32, __33, __34, __41, __42, __43, __44);
		}

		Matrix4x4(const Matrix4x4& other)
		{
			_11 = other._11; _12 = other._12; _13 = other._13; _14 = other._14;
			_21 = other._21; _22 = other._22; _23 = other._23; _24 = other._24;
			_31 = other._31; _32 = other._32; _33 = other._33; _34 = other._34;
			_41 = other._41; _42 = other._42; _43 = other._43; _44 = other._44;
		}

		Matrix4x4(const Matrix3D<T>& other)
		{
			_11 = other._11; _12 = other._12; _13 = other._13; _14 = other._14;
			_21 = other._21; _22 = other._22; _23 = other._23; _24 = other._24;
			_31 = other._31; _32 = other._32; _33 = other._33; _34 = other._34;
			_41 = other._41; _42 = other._42; _43 = other._43; _44 = other._44;
		}

		inline void SetIdentity(void)
		{
			memset(this, 0, sizeof(Matrix4x4));
			_11 = _22 = _33 = _44 = (T)1;
		}

		inline void Set(T __11, T __12, T __13, T __14, T __21, T __22, T __23, T __24, T __31, T __32, T __33, T __34, T __41, T __42, T __43, T __44)
		{
			_11 = __11;
			_12 = __12;
			_13 = __13;
			_14 = __14;
			_21 = __21;
			_22 = __22;
			_23 = __23;
			_24 = __24;
			_31 = __31;
			_32 = __32;
			_33 = __33;
			_34 = __34;
			_41 = __41;
			_42 = __42;
			_43 = __43;
			_44 = __44;
		}


		inline T Determinant(void) const
		{
			return _11*_22*_33*_44 + _11*_23*_34*_42 + _11*_24*_32*_43 
					+ _12*_21*_34*_43 + _12*_23*_31*_44 + _12*_24*_33*_41
					+ _13*_21*_32*_44 + _13*_22*_34*_41 + _13*_24*_31*_42
					+ _14*_21*_33*_42 + _14*_22*_31*_43 + _14*_23*_32*_41
					- _11*_22*_34*_43 - _11*_23*_32*_44 - _11*_24*_33*_42
					- _12*_21*_33*_44 - _12*_23*_34*_41 - _12*_24*_31*_43
					- _13*_21*_34*_42 - _13*_22*_31*_44 - _13*_24*_32*_41
					- _14*_21*_32*_43 - _14*_22*_33*_41 - _14*_23*_31*_42;
		}

		inline Matrix4x4 Adjugate(void) const
		{
			return Matrix4x4(
				_22*_33*_44 + _23*_34*_42 + _24*_32*_43 - _22*_34*_43 - _23*_32*_44 - _24*_33*_42,
				_12*_34*_43 + _13*_32*_44 + _14*_33*_42 - _12*_33*_44 - _13*_34*_42 - _14*_32*_43,
				_12*_23*_44 + _13*_24*_42 + _14*_22*_43 - _12*_24*_43 - _13*_22*_44 - _14*_23*_42,
				_12*_24*_33 + _13*_22*_34 + _14*_23*_32 - _12*_23*_34 - _13*_24*_32 - _14*_22*_33,
				_21*_34*_43 + _23*_31*_44 + _24*_33*_41 - _21*_33*_44 - _23*_34*_41 - _24*_31*_43,
				_11*_33*_44 + _13*_32*_41 + _14*_31*_43 - _11*_34*_43 - _13*_31*_44 - _14*_33*_41,
				_11*_24*_43 + _13*_21*_44 + _14*_23*_41 - _11*_23*_44 - _13*_24*_41 - _14*_21*_43,
				_11*_23*_34 + _13*_24*_31 + _14*_21*_33 - _11*_24*_33 - _13*_21*_34 - _14*_23*_31,
				_21*_32*_44 + _22*_34*_41 + _24*_31*_42 - _21*_34*_42 - _22*_31*_44 - _24*_32*_41,
				_11*_34*_42 + _12*_31*_44 + _14*_32*_41 - _11*_32*_44 - _12*_34*_41 - _14*_31*_42,
				_11*_22*_44 + _12*_24*_41 + _14*_21*_42 - _11*_24*_42 - _12*_21*_44 - _14*_22*_41,
				_11*_24*_32 + _12*_21*_34 + _14*_22*_31 - _11*_22*_34 - _12*_24*_31 - _14*_21*_32,
				_21*_33*_42 + _22*_31*_43 + _23*_32*_41 - _21*_32*_43 - _22*_33*_41 - _23*_31*_42,
				_11*_32*_43 + _12*_33*_41 + _13*_31*_42 - _11*_33*_42 - _12*_31*_43 - _13*_32*_41,
				_11*_23*_42 + _12*_21*_43 + _13*_22*_41 - _11*_22*_43 - _12*_23*_41 - _13*_21*_42,
				_11*_22*_33 + _12*_23*_31 + _13*_21*_32 - _11*_23*_32 - _12*_21*_33 - _13*_22*_31
				);
		}

		inline Matrix4x4 Inverse(void) const
		{
			return ((T)1 / Determinant()) * Adjugate();
		}

		inline void Transpose(void)
		{
			Set(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44);
		}

		inline Matrix4x4 Transposed(void) const
		{
			return Matrix4x4(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44);
		}

		inline void Scale(T a)
		{
			(*this) *= Matrix4x4((T)a, (T)0, (T)0, (T)0,
								(T)0, (T)a, (T)0, (T)0,
								(T)0, (T)0, (T)a, (T)0,
								(T)0, (T)0, (T)0, (T)a);
		}

		inline Matrix4x4 operator * (const Matrix4x4& other) const
		{
			return Matrix4x4(
				_11 * other._11 + _12 * other._21 + _13 * other._31 + _14 * other._41,
				_11 * other._12 + _12 * other._22 + _13 * other._32 + _14 * other._42,
				_11 * other._13 + _12 * other._23 + _13 * other._33 + _14 * other._43,
				_11 * other._14 + _12 * other._24 + _13 * other._34 + _14 * other._44,

				_21 * other._11 + _22 * other._21 + _23 * other._31 + _24 * other._41,
				_21 * other._12 + _22 * other._22 + _23 * other._32 + _24 * other._42,
				_21 * other._13 + _22 * other._23 + _23 * other._33 + _24 * other._43,
				_21 * other._14 + _22 * other._24 + _23 * other._34 + _24 * other._44,

				_31 * other._11 + _32 * other._21 + _33 * other._31 + _34 * other._41,
				_31 * other._12 + _32 * other._22 + _33 * other._32 + _34 * other._42,
				_31 * other._13 + _32 * other._23 + _33 * other._33 + _34 * other._43,
				_31 * other._14 + _32 * other._24 + _33 * other._34 + _34 * other._44,

				_41 * other._11 + _42 * other._21 + _43 * other._31 + _44 * other._41,
				_41 * other._12 + _42 * other._22 + _43 * other._32 + _44 * other._42,
				_41 * other._13 + _42 * other._23 + _43 * other._33 + _44 * other._43,
				_41 * other._14 + _42 * other._24 + _43 * other._34 + _44 * other._44
				);
		}

		inline Vector4<T> operator * (const Vector4<T>& vector) const
		{
			return Vector4<T>(
				_11 * vector.x + _12 * vector.y + _13 * vector.z + _14 * vector.w,
				_21 * vector.x + _22 * vector.y + _23 * vector.z + _24 * vector.w,
				_31 * vector.x + _32 * vector.y + _33 * vector.z + _34 * vector.w,
				_41 * vector.x + _42 * vector.y + _43 * vector.z + _44 * vector.w
				);
		}

		inline Matrix4x4 operator * (const T& scalar) const
		{
			return Matrix4x4(
				_11 * scalar, _12 * scalar, _13 * scalar, _14 * scalar,
				_21 * scalar, _22 * scalar, _23 * scalar, _24 * scalar,
				_31 * scalar, _32 * scalar, _33 * scalar, _34 * scalar,
				_41 * scalar, _42 * scalar, _43 * scalar, _44 * scalar
				);
		}

		inline void operator *= (const Matrix4x4& other)
		{
			Set(
				_11 * other._11 + _12 * other._21 + _13 * other._31 + _14 * other._41,
				_11 * other._12 + _12 * other._22 + _13 * other._32 + _14 * other._42,
				_11 * other._13 + _12 * other._23 + _13 * other._33 + _14 * other._43,
				_11 * other._14 + _12 * other._24 + _13 * other._34 + _14 * other._44,

				_21 * other._11 + _22 * other._21 + _23 * other._31 + _24 * other._41,
				_21 * other._12 + _22 * other._22 + _23 * other._32 + _24 * other._42,
				_21 * other._13 + _22 * other._23 + _23 * other._33 + _24 * other._43,
				_21 * other._14 + _22 * other._24 + _23 * other._34 + _24 * other._44,

				_31 * other._11 + _32 * other._21 + _33 * other._31 + _34 * other._41,
				_31 * other._12 + _32 * other._22 + _33 * other._32 + _34 * other._42,
				_31 * other._13 + _32 * other._23 + _33 * other._33 + _34 * other._43,
				_31 * other._14 + _32 * other._24 + _33 * other._34 + _34 * other._44,

				_41 * other._11 + _42 * other._21 + _43 * other._31 + _44 * other._41,
				_41 * other._12 + _42 * other._22 + _43 * other._32 + _44 * other._42,
				_41 * other._13 + _42 * other._23 + _43 * other._33 + _44 * other._43,
				_41 * other._14 + _42 * other._24 + _43 * other._34 + _44 * other._44
				);
		}

		inline void operator *= (const T& scalar)
		{
			Set(
				_11 * scalar, _12 * scalar, _13 * scalar, _14 * scalar,
				_21 * scalar, _22 * scalar, _23 * scalar, _24 * scalar,
				_31 * scalar, _32 * scalar, _33 * scalar, _34 * scalar,
				_41 * scalar, _42 * scalar, _43 * scalar, _44 * scalar
				);
		}

		inline bool operator == (const Matrix4x4& other) const
		{
			return _11 == other._11 && _12 == other._12 && _13 == other._13 && _14 == other._14
				&& _21 == other._21 && _22 == other._22 && _23 == other._23 && _24 == other._24
				&& _31 == other._31 && _32 == other._32 && _33 == other._33 && _34 == other._34
				&& _41 == other._41 && _42 == other._42 && _43 == other._43 && _44 == other._44;
		}

		inline bool operator != (const Matrix4x4& other) const
		{
			return _11 != other._11 || _12 != other._12 || _13 != other._13 || _14 != other._14
				|| _21 != other._21 || _22 != other._22 || _23 != other._23 || _24 != other._24
				|| _31 != other._31 || _32 != other._32 || _33 != other._33 || _34 != other._34
				|| _41 != other._41 || _42 != other._42 || _43 != other._43 || _44 != other._44;
		}

		template <typename C>
		inline operator Matrix4x4<C>()
		{
			return Matrix4x4<C>((C)_11, (C)_12, (C)_13, (C)_14, (C)_21, (C)_22, (C)_23, (C)_24, (C)_31, (C)_32, (C)_33, (C)_34, (C)_41, (C)_42, (C)_43, (C)_44);
		}

		template <typename C>
		inline operator Matrix3D<C>()
		{
			return Matrix3D<C>((C)_11, (C)_12, (C)_13, (C)_14, (C)_21, (C)_22, (C)_23, (C)_24, (C)_31, (C)_32, (C)_33, (C)_34, (C)_41, (C)_42, (C)_43, (C)_44);
		}
	};
	/*----------------------------------------------------------------*/

	template <typename T> inline Matrix4x4<T> operator *(float s, const Matrix4x4<T>& matrix)
	{
		return matrix * s;
	}
}
