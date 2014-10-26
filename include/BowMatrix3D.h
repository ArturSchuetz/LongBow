#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector3.h"
#include "BowVector4.h"

namespace Bow {
	namespace Core {

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

			Matrix3D(T __11, T __12, T __13, T __21, T __22, T __23, T __31, T __32, T __33)
			{
				_14 = = _24 = _34 = _41 = _42 = _43 = (T)0;
				_44 = (T)1;

				Set(T__11, __12, __13, __21, __22, __23, __31, __32, __33);
			}

			Matrix3D(T __11, T __12, T __13, T __14, T __21, T __22, T __23, T __24, T __31, T __32, T __33, T __34, T __41, T __42, T __43, T __44)
			{
				Set(__11, __12, __13, __14, __21, __22, __23, __24, __31, __32, __33, __34, __41, __42, __43, __44);
			}

			inline void SetIdentity(void)
			{
				memset(this, 0, sizeof(Matrix3D));
				_11 = _22 = _33 = _44 = (T)1;
			}

			inline void Set(T __11, T __12, T __13, T __21, T __22, T __23, T __31, T __32, T __33)
			{
				_11 = __11;
				_12 = __12;
				_13 = __13;
				_21 = __21;
				_22 = __22;
				_23 = __23;
				_31 = __31;
				_32 = __32;
				_33 = __33;
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

			inline void SetTransformation(T __11, T __12, T __21, T __22);

			inline void SetTranslation(const Vector3<T>& vector);
			inline Vector3<T> GetTranslation(void) const;
			inline T Determinant(void) const;
			inline Matrix3D Adjugate(void) const;
			inline Matrix3D Inverse(void) const;

			inline void Transpose(void)
			{
				Set(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44);
			}

			inline void Translate(const Vector3<T>& vector);

			/* ToDo: Lineare Rotationsinterpolation zwischen 2 winkeln und Translationsinterpolation */
			inline void Rotate(T a);
			inline void ScaleX(T a);
			inline void ScaleY(T a);
			inline void Scale(T a);

			template <typename C>
			inline Matrix3D operator * (const Matrix3D<C>& other) const
			{
				return Matrix3D(
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

			inline Vector3<T> operator * (const Vector3<T>& vector) const;
			inline Vector4<T> operator * (const Vector4<T>& vector) const;
			inline Matrix3D operator * (const T& scalar) const;

			inline void operator *= (const Matrix3D& other);
			inline void operator *= (const T& scalar) const;
			inline bool operator == (const Matrix3D& other) const;
			inline bool operator != (const Matrix3D& other) const;

			template <typename C>
			inline operator Matrix3D<C>()
			{
				return Matrix3D<C>((C)_11, (C)_12, (C)_13, (C)_14, (C)_21, (C)_22, (C)_23, (C)_24, (C)_31, (C)_32, (C)_33, (C)_34, (C)_41, (C)_42, (C)_43, (C)_44);
			}
		};
		/*----------------------------------------------------------------*/

		class FloatMatrix3D : public Matrix3D<float>
		{
		public:
			FloatMatrix3D()
			{
				SetIdentity();
			}

			template <typename C>
			FloatMatrix3D(const Matrix3D<C>& other)
			{
				Set((C)other._11, (C)other._12, (C)other._13, (C)other._14, (C)other._21, (C)other._22, (C)other._23, (C)other._24, (C)other._31, (C)other._32, (C)other._33, (C)other._34, (C)other._41, (C)other._42, (C)other._43, (C)other._44);
			}
		};
		/*----------------------------------------------------------------*/
	}
}