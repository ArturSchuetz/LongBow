/**
 * @file BowMatrix3D.h
 * @brief Declarations for BowMatrix3D.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"
#include "BowVector4.h"

namespace bow {

	template<typename T>
	class Matrix3D
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

		Matrix3D(void)
		{
			SetIdentity();
		}

		template <typename C>
		Matrix3D(C __11, C __12, C __13, C __21, C __22, C __23, C __31, C __32, C __33)
		{
			_14 = _24 = _34 = _41 = _42 = _43 = (T)0;
			_44 = (T)1;

			Set((T)__11, (T)__12, (T)__13, (T)__21, (T)__22, (T)__23, (T)__31, (T)__32, (T)__33);
		}

		template <typename C>
		Matrix3D(C __11, C __12, C __13, C __14, C __21, C __22, C __23, C __24, C __31, C __32, C __33, C __34, C __41, C __42, C __43, C __44)
		{
			Set((T)__11, (T)__12, (T)__13, (T)__14, (T)__21, (T)__22, (T)__23, (T)__24, (T)__31, (T)__32, (T)__33, (T)__34, (T)__41, (T)__42, (T)__43, (T)__44);
		}

		template <typename C>
		Matrix3D(const Matrix3D<C>& other)
		{
			_11 = (T)other._11; _12 = (T)other._12; _13 = (T)other._13; _14 = (T)other._14;
			_21 = (T)other._21; _22 = (T)other._22; _23 = (T)other._23; _24 = (T)other._24;
			_31 = (T)other._31; _32 = (T)other._32; _33 = (T)other._33; _34 = (T)other._34;
			_41 = (T)other._41; _42 = (T)other._42; _43 = (T)other._43; _44 = (T)other._44;
		}

		template <typename C>
		Matrix3D(const Matrix4x4<C>& other)
		{
			_11 = (T)other._11; _12 = (T)other._12; _13 = (T)other._13; _14 = (T)other._14;
			_21 = (T)other._21; _22 = (T)other._22; _23 = (T)other._23; _24 = (T)other._24;
			_31 = (T)other._31; _32 = (T)other._32; _33 = (T)other._33; _34 = (T)other._34;
			_41 = (T)other._41; _42 = (T)other._42; _43 = (T)other._43; _44 = (T)other._44;
		}

		inline void SetIdentity(void)
		{
			memset(this, 0, sizeof(Matrix3D));
			_11 = _22 = _33 = _44 = (T)1;
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
			_44 = (T)1;
		}

		template <typename C>
		inline void Set(C __11, C __12, C __13, C __14, C __21, C __22, C __23, C __24, C __31, C __32, C __33, C __34, C __41, C __42, C __43, C __44)
		{
			_11 = (T)__11;
			_12 = (T)__12;
			_13 = (T)__13;
			_14 = (T)__14;
			_21 = (T)__21;
			_22 = (T)__22;
			_23 = (T)__23;
			_24 = (T)__24;
			_31 = (T)__31;
			_32 = (T)__32;
			_33 = (T)__33;
			_34 = (T)__34;
			_41 = (T)__41;
			_42 = (T)__42;
			_43 = (T)__43;
			_44 = (T)__44;
		}

		template <typename C>
		inline void SetTranslation(const Vector3<C>& vector)
		{
			_14 = (T)vector.x;
			_24 = (T)vector.y;
			_34 = (T)vector.z;
		}

		template <typename C>
		inline void SetTranslation(const C& x, const C& y, const C& z)
		{
			_14 = (T)x;
			_24 = (T)y;
			_34 = (T)z;
		}

		inline Vector3<T> GetTranslation(void) const
		{
			return Vector3<T>(_14, _24, _34);
		}

		inline T Determinant(void) const
		{
			return _11*_22*_33 + _12*_23*_31 + _13*_21*_32 - _13*_22*_31 - _12*_21*_33 - _11*_23*_32;
		}

		inline Matrix3D Adjugate(void) const
		{
			return Matrix3D(_22*_33 - _23*_32, _13*_32 - _12*_33, _12*_23 - _13*_22,
							_23*_31 - _21*_33, _11*_33 - _13*_31, _13*_21 - _11*_23,
							_21*_32 - _22*_31, _12*_31 - _11*_32, _11*_22 - _12*_21);
		}

		inline Matrix3D Inverse(void) const
		{
			Matrix3D inv = Adjugate() * ((T)1 / Determinant());
			Vector3<T> Translation = inv * Vector3<T>(_14, _24, _34);
			inv._14 = -Translation.x;
			inv._24 = -Translation.y;
			inv._34 = -Translation.z;
			inv._44 = (T)1;
			return inv;
		}

		inline void Transpose(void)
		{
			Set(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44);
		}

		inline Matrix3D Transposed(void) const
		{
			return Matrix3D(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44);
		}

		template <typename C>
		inline void Translate(const Vector3<C>& vector)
		{
			_14 += (T)vector.x;
			_24 += (T)vector.y;
			_34 += (T)vector.z;
		}

		template <typename C>
		inline void RotateX(C a)
		{
			T Cos = (T)cos(a);
			T Sin = (T)sin(a);
			(*this) *= Matrix3D((T)1, (T)0, (T)0,
								(T)0, Cos, -Sin,
								(T)0, Sin, Cos);
		}

		template <typename C>
		inline void RotateY(C a)
		{
			T Cos = (T)cos(a);
			T Sin = (T)sin(a);
			(*this) *= Matrix3D(Cos, (T)0, Sin,
								(T)0, (T)1, (T)0,
								-Sin, (T)0, Cos);
		}

		template <typename C>
		inline void RotateZ(C a)
		{
			T Cos = (T)cos(a);
			T Sin = (T)sin(a);
			(*this) *= Matrix3D(Cos, -Sin, (T)0,
				Sin, Cos, (T)0,
				(T)0, (T)0, (T)1);
		}

		template <typename C>
		inline void Scale(C a)
		{
			(*this) *= Matrix3D((T)a, (T)0, (T)0,
								(T)0, (T)a, (T)0,
								(T)0, (T)0, (T)a);
			(*this)._14 *= (T)a;
			(*this)._24 *= (T)a;
			(*this)._34 *= (T)a;
		}

		template <typename C>
		inline Matrix3D operator * (const Matrix3D<C>& other) const
		{
			return Matrix3D(
				_11 * (T)other._11 + _12 * (T)other._21 + _13 * (T)other._31 + _14 * (T)other._41,
				_11 * (T)other._12 + _12 * (T)other._22 + _13 * (T)other._32 + _14 * (T)other._42,
				_11 * (T)other._13 + _12 * (T)other._23 + _13 * (T)other._33 + _14 * (T)other._43,
				_11 * (T)other._14 + _12 * (T)other._24 + _13 * (T)other._34 + _14 * (T)other._44,

				_21 * (T)other._11 + _22 * (T)other._21 + _23 * (T)other._31 + _24 * (T)other._41,
				_21 * (T)other._12 + _22 * (T)other._22 + _23 * (T)other._32 + _24 * (T)other._42,
				_21 * (T)other._13 + _22 * (T)other._23 + _23 * (T)other._33 + _24 * (T)other._43,
				_21 * (T)other._14 + _22 * (T)other._24 + _23 * (T)other._34 + _24 * (T)other._44,

				_31 * (T)other._11 + _32 * (T)other._21 + _33 * (T)other._31 + _34 * (T)other._41,
				_31 * (T)other._12 + _32 * (T)other._22 + _33 * (T)other._32 + _34 * (T)other._42,
				_31 * (T)other._13 + _32 * (T)other._23 + _33 * (T)other._33 + _34 * (T)other._43,
				_31 * (T)other._14 + _32 * (T)other._24 + _33 * (T)other._34 + _34 * (T)other._44,

				_41 * (T)other._11 + _42 * (T)other._21 + _43 * (T)other._31 + _44 * (T)other._41,
				_41 * (T)other._12 + _42 * (T)other._22 + _43 * (T)other._32 + _44 * (T)other._42,
				_41 * (T)other._13 + _42 * (T)other._23 + _43 * (T)other._33 + _44 * (T)other._43,
				_41 * (T)other._14 + _42 * (T)other._24 + _43 * (T)other._34 + _44 * (T)other._44
			);
		}

		template <typename C>
		inline Vector3<T> operator * (const Vector3<C>& vector) const
		{
			return Vector3<T>(
				_11 * (T)vector.x + _12 * (T)vector.y + _13 * (T)vector.z + _14,
				_21 * (T)vector.x + _22 * (T)vector.y + _23 * (T)vector.z + _24,
				_31 * (T)vector.x + _32 * (T)vector.y + _33 * (T)vector.z + _34
				);
		}

		template <typename C>
		inline Vector4<T> operator * (const Vector4<C>& vector) const
		{
			return Vector4<T>(
				_11 * (T)vector.x + _12 * (T)vector.y + _13 * (T)vector.z + _14 * (T)vector.w,
				_21 * (T)vector.x + _22 * (T)vector.y + _23 * (T)vector.z + _24 * (T)vector.w,
				_31 * (T)vector.x + _32 * (T)vector.y + _33 * (T)vector.z + _34 * (T)vector.w,
				_41 * (T)vector.x + _42 * (T)vector.y + _43 * (T)vector.z + _44 * (T)vector.w
				);
		}

		template <typename C>
		inline Matrix3D operator * (const C& _scalar) const
		{
			T scalar = (T)_scalar;
			return Matrix3D(
				_11 * scalar, _12 * scalar, _13 * scalar, _14 * scalar,
				_21 * scalar, _22 * scalar, _23 * scalar, _24 * scalar,
				_31 * scalar, _32 * scalar, _33 * scalar, _34 * scalar,
				_41 * scalar, _42 * scalar, _43 * scalar, _44 * scalar
			);
		}

		template <typename C>
		inline void operator *= (const Matrix3D<C>& other)
		{
			Set(
				_11 * (T)other._11 + _12 * (T)other._21 + _13 * (T)other._31 + _14 * (T)other._41,
				_11 * (T)other._12 + _12 * (T)other._22 + _13 * (T)other._32 + _14 * (T)other._42,
				_11 * (T)other._13 + _12 * (T)other._23 + _13 * (T)other._33 + _14 * (T)other._43,
				_11 * (T)other._14 + _12 * (T)other._24 + _13 * (T)other._34 + _14 * (T)other._44,

				_21 * (T)other._11 + _22 * (T)other._21 + _23 * (T)other._31 + _24 * (T)other._41,
				_21 * (T)other._12 + _22 * (T)other._22 + _23 * (T)other._32 + _24 * (T)other._42,
				_21 * (T)other._13 + _22 * (T)other._23 + _23 * (T)other._33 + _24 * (T)other._43,
				_21 * (T)other._14 + _22 * (T)other._24 + _23 * (T)other._34 + _24 * (T)other._44,

				_31 * (T)other._11 + _32 * (T)other._21 + _33 * (T)other._31 + _34 * (T)other._41,
				_31 * (T)other._12 + _32 * (T)other._22 + _33 * (T)other._32 + _34 * (T)other._42,
				_31 * (T)other._13 + _32 * (T)other._23 + _33 * (T)other._33 + _34 * (T)other._43,
				_31 * (T)other._14 + _32 * (T)other._24 + _33 * (T)other._34 + _34 * (T)other._44,

				_41 * (T)other._11 + _42 * (T)other._21 + _43 * (T)other._31 + _44 * (T)other._41,
				_41 * (T)other._12 + _42 * (T)other._22 + _43 * (T)other._32 + _44 * (T)other._42,
				_41 * (T)other._13 + _42 * (T)other._23 + _43 * (T)other._33 + _44 * (T)other._43,
				_41 * (T)other._14 + _42 * (T)other._24 + _43 * (T)other._34 + _44 * (T)other._44
			);
		}

		template <typename C>
		inline void operator *= (const C& _scalar) const
		{
			T scalar = (T)_scalar;
			Set(
				_11 * scalar, _12 * scalar, _13 * scalar, _14 * scalar,
				_21 * scalar, _22 * scalar, _23 * scalar, _24 * scalar,
				_31 * scalar, _32 * scalar, _33 * scalar, _34 * scalar,
				_41 * scalar, _42 * scalar, _43 * scalar, _44 * scalar
			);
		}

		template <typename C>
		inline bool operator == (const Matrix3D<C>& other) const
		{
			return _11 == (T)other._11 && _12 == (T)other._12 && _13 == (T)other._13 && _14 == (T)other._14
				&& _21 == (T)other._21 && _22 == (T)other._22 && _23 == (T)other._23 && _24 == (T)other._24
				&& _31 == (T)other._31 && _32 == (T)other._32 && _33 == (T)other._33 && _34 == (T)other._34
				&& _41 == (T)other._41 && _42 == (T)other._42 && _43 == (T)other._43 && _44 == (T)other._44;
		}

		template <typename C>
		inline bool operator != (const Matrix3D<C>& other) const
		{
			return _11 != (T)other._11 || _12 != (T)other._12 || _13 != (T)other._13 || _14 != (T)other._14
				|| _21 != (T)other._21 || _22 != (T)other._22 || _23 != (T)other._23 || _24 != (T)other._24
				|| _31 != (T)other._31 || _32 != (T)other._32 || _33 != (T)other._33 || _34 != (T)other._34
				|| _41 != (T)other._41 || _42 != (T)other._42 || _43 != (T)other._43 || _44 != (T)other._44;
		}

		template <typename C>
		inline operator Matrix3D<C>()
		{
			return Matrix3D<C>((C)_11, (C)_12, (C)_13, (C)_14, (C)_21, (C)_22, (C)_23, (C)_24, (C)_31, (C)_32, (C)_33, (C)_34, (C)_41, (C)_42, (C)_43, (C)_44);
		}

		template <typename C>
		inline operator Matrix4x4<C>()
		{
			return Matrix4x4<C>((C)_11, (C)_12, (C)_13, (C)_14, (C)_21, (C)_22, (C)_23, (C)_24, (C)_31, (C)_32, (C)_33, (C)_34, (C)_41, (C)_42, (C)_43, (C)_44);
		}
	};
	/*----------------------------------------------------------------*/

	template <typename T> inline Matrix3D<T> operator *(float s, const Matrix3D<T>& matrix)
	{
		return matrix * s;
	}
}
