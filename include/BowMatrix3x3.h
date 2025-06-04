#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"

namespace bow {

	template<typename T>
	class Matrix3x3
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

		static Matrix3x3 Identity(void) {
			return Matrix3x3((T)1, (T)0, (T)0, (T)0, (T)1, (T)0, (T)0, (T)0, (T)1);
		}

		Matrix3x3(void)
		{
			SetIdentity();
		}

		template <typename C>
		Matrix3x3(C __11, C __12, C __13, C __21, C __22, C __23, C __31, C __32, C __33)
		{
			Set((T)__11, (T)__12, (T)__13, (T)__21, (T)__22, (T)__23, (T)__31, (T)__32, (T)__33);
		}

		template <typename C>
		Matrix3x3(const Matrix3x3<C>& other)
		{
			_11 = (T)other._11; _12 = (T)other._12; _13 = (T)other._13;
			_21 = (T)other._21; _22 = (T)other._22; _23 = (T)other._23;
			_31 = (T)other._31; _32 = (T)other._32; _33 = (T)other._33;
		}

		inline void SetIdentity(void)
		{
			memset(this, 0, sizeof(Matrix3x3));
			_11 = _22 = _33 = (T)1;
		}

		template <typename C>
		inline void Set(C __11, C __12, C __13, C __21, C __22, C __23, C __31, C __32, C __33)
		{
			_11 = (T)__11;
			_12 = (T)__12;
			_13 = (T)__13;
			_21 = (T)__21;
			_22 = (T)__22;
			_23 = (T)__23;
			_31 = (T)__31;
			_32 = (T)__32;
			_33 = (T)__33;
		}

		inline T Determinant(void) const
		{
			return _11*_22*_33 + _12*_23*_31 + _13*_21*_32 - _13*_22*_31 - _12*_21*_33 - _11*_23*_32;
		}

		inline Matrix3x3 Adjugate(void) const
		{
			return Matrix3x3(_22*_33 - _23*_32, _13*_32 - _12*_33, _12*_23 - _13*_22,
							 _23*_31 - _21*_33, _11*_33 - _13*_31, _13*_21 - _11*_23,
							 _21*_32 - _22*_31, _12*_31 - _11*_32, _11*_22 - _12*_21);
		}

		inline Matrix3x3 Inverse(void) const
		{
			Matrix3x3 inv = Adjugate() * ((T)1 / Determinant());
			return inv;
		}

		inline void Transpose(void)
		{
			Set(_11, _21, _31, _12, _22, _32, _13, _23, _33);
		}

		inline Matrix3x3 Transposed(void) const
		{
			return Matrix3x3(_11, _21, _31, _12, _22, _32, _13, _23, _33);
		}

		template <typename C>
		inline void RotateX(C a)
		{
			T Cos = (T)cos(a);
			T Sin = (T)sin(a);
			(*this) *= Matrix3x3((T)1, (T)0, (T)0,
								 (T)0, Cos, -Sin,
								 (T)0, Sin, Cos);
		}

		template <typename C>
		inline void RotateY(C a)
		{
			T Cos = (T)cos(a);
			T Sin = (T)sin(a);
			(*this) *= Matrix3x3(Cos, (T)0, Sin,
								 (T)0, (T)1, (T)0,
								 -Sin, (T)0, Cos);
		}

		template <typename C>
		inline void RotateZ(C a)
		{
			T Cos = (T)cos(a);
			T Sin = (T)sin(a);
			(*this) *= Matrix3x3(Cos, -Sin, (T)0,
								 Sin, Cos, (T)0,
								 (T)0, (T)0, (T)1);
		}

		template <typename C>
		inline void Scale(C a)
		{
			(*this) *= Matrix3x3((T)a, (T)0, (T)0,
								 (T)0, (T)a, (T)0,
								 (T)0, (T)0, (T)a);
		}

		template <typename C>
		inline Matrix3x3 operator * (const Matrix3x3<C>& other) const
		{
			return Matrix3x3(
				_11 * (T)other._11 + _12 * (T)other._21 + _13 * (T)other._31,
				_11 * (T)other._12 + _12 * (T)other._22 + _13 * (T)other._32,
				_11 * (T)other._13 + _12 * (T)other._23 + _13 * (T)other._33,

				_21 * (T)other._11 + _22 * (T)other._21 + _23 * (T)other._31,
				_21 * (T)other._12 + _22 * (T)other._22 + _23 * (T)other._32,
				_21 * (T)other._13 + _22 * (T)other._23 + _23 * (T)other._33,

				_31 * (T)other._11 + _32 * (T)other._21 + _33 * (T)other._31,
				_31 * (T)other._12 + _32 * (T)other._22 + _33 * (T)other._32,
				_31 * (T)other._13 + _32 * (T)other._23 + _33 * (T)other._33
			);
		}

		template <typename C>
		inline Vector3<T> operator * (const Vector3<C>& vector) const
		{
			return Vector3<T>(
				_11 * (T)vector.x + _12 * (T)vector.y + _13 * (T)vector.z,
				_21 * (T)vector.x + _22 * (T)vector.y + _23 * (T)vector.z,
				_31 * (T)vector.x + _32 * (T)vector.y + _33 * (T)vector.z
				);
		}

		template <typename C>
               inline Matrix3x3 operator * (const C& _scalar) const
               {
                       T scalar = (T)_scalar;
                       return Matrix3x3(
                               _11 * scalar, _12 * scalar, _13 * scalar,
                               _21 * scalar, _22 * scalar, _23 * scalar,
                               _31 * scalar, _32 * scalar, _33 * scalar
                       );
               }

		template <typename C>
		inline void operator *= (const Matrix3x3<C>& other)
		{
			Set(
				_11 * (T)other._11 + _12 * (T)other._21 + _13 * (T)other._31,
				_11 * (T)other._12 + _12 * (T)other._22 + _13 * (T)other._32,
				_11 * (T)other._13 + _12 * (T)other._23 + _13 * (T)other._33,

				_21 * (T)other._11 + _22 * (T)other._21 + _23 * (T)other._31,
				_21 * (T)other._12 + _22 * (T)other._22 + _23 * (T)other._32,
				_21 * (T)other._13 + _22 * (T)other._23 + _23 * (T)other._33,

				_31 * (T)other._11 + _32 * (T)other._21 + _33 * (T)other._31,
				_31 * (T)other._12 + _32 * (T)other._22 + _33 * (T)other._32,
				_31 * (T)other._13 + _32 * (T)other._23 + _33 * (T)other._33
			);
		}

		template <typename C>
		inline void operator *= (const C& _scalar) const
		{
			T scalar = (T)_scalar;
			Set(
				_11 * scalar, _12 * scalar, _13 * scalar,
				_21 * scalar, _22 * scalar, _23 * scalar,
				_31 * scalar, _32 * scalar, _33 * scalar
			);
		}

		template <typename C>
		inline bool operator == (const Matrix3x3<C>& other) const
		{
			return _11 == (T)other._11 && _12 == (T)other._12 && _13 == (T)other._13
				&& _21 == (T)other._21 && _22 == (T)other._22 && _23 == (T)other._23
				&& _31 == (T)other._31 && _32 == (T)other._32 && _33 == (T)other._33;
		}

		template <typename C>
		inline bool operator != (const Matrix3x3<C>& other) const
		{
			return _11 != (T)other._11 || _12 != (T)other._12 || _13 != (T)other._13
				|| _21 != (T)other._21 || _22 != (T)other._22 || _23 != (T)other._23
				|| _31 != (T)other._31 || _32 != (T)other._32 || _33 != (T)other._33;
		}

		template <typename C>
		inline operator Matrix3x3<C>()
		{
			return Matrix3x3<C>((C)_11, (C)_12, (C)_13, (C)_21, (C)_22, (C)_23, (C)_31, (C)_32, (C)_33);
		}

	};
	/*----------------------------------------------------------------*/

	template <typename T> inline Matrix3x3<T> operator *(float s, const Matrix3x3<T>& matrix)
	{
		return matrix * s;
	}
}
