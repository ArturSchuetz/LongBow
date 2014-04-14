#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector2.h"

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
			Matrix3D(T __11, T __12, T __13, T __21);
			Matrix3D(T __11, T __12, T __13, T __21, T __22, T __23);
			Matrix3D(T __11, T __12, T __13, T __21, T __22, T __23, T __31, T __32, T __33);

			inline void SetIdentity(void);

			inline void Set(T __11, T __12, T __13, T __21, T __22, T __23, T __31, T __32, T __33);
			inline void Set(T __11, T __12, T __13, T __21, T __22, T __23);
			inline void SetTransformation(T __11, T __12, T __21, T __22);

			inline void SetTranslation(const Vector2<T>& vector);
			inline Vector2<T> GetTranslation(void) const;
			inline T Determinant(void) const;
			inline Matrix3D Adjugate(void) const;
			inline Matrix3D Inverse(void) const;
			inline void Transpose(void);
			inline void Translate(const Vector2<T>& vector);
			inline void Rotate(T a);
			inline void ScaleX(T a);
			inline void ScaleY(T a);
			inline void Scale(T a);

			inline Matrix3D operator * (const Matrix3D& other) const;
			inline Vector3<T> operator * (const Vector3<T>& vector) const;
			inline Matrix3D operator * (const T& scalar) const;

			inline void operator *= (const Matrix3D& other);
			inline void operator *= (const T& scalar) const;
			inline bool operator == (const Matrix3D& other) const;
			inline bool operator != (const Matrix3D& other) const;
		};
		/*----------------------------------------------------------------*/
	}
}