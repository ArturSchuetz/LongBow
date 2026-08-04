#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Math/BowVector2.h"

namespace bow
{
namespace Core
{

template <typename T> class Matrix2x2
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

    Matrix2x2()
    {
        FN("Matrix2x2::Matrix2x2");

        SetIdentity();
    }

    Matrix2x2(T __11, T __12, T __21, T __22)
    {
        FN("Matrix2x2::Matrix2x2");

        _11 = __11;
        _12 = __12;
        _21 = __21;
        _22 = __22;
    }

    inline void SetIdentity()
    {
        FN("Matrix2x2::SetIdentity");

        memset(this, 0, sizeof(Matrix2x2));
        _11 = _22 = (T)1;
    }

    inline void Set(T __11, T __12, T __21, T __22)
    {
        FN("Matrix2x2::Set");

        _11 = __11;
        _12 = __12;
        _21 = __21;
        _22 = __22;
    }

    inline T Determinant() const
    {
        FN("Matrix2x2::Determinant");

        return _11 * _22 - _12 * _21;
    }

    inline Matrix2x2 Adjugate() const
    {
        FN("Matrix2x2::Adjugate");

        return Matrix2x2(_22, -_12, -_21, _11);
    }

    inline Matrix2x2 Inverse() const
    {
        FN("Matrix2x2::Inverse");

        return Adjugate() * ((T)1 / Determinant());
    }

    inline void Transpose()
    {
        FN("Matrix2x2::Transpose");

        Set(_11, _21, _12, _22);
    }

    inline void Rotate(T a)
    {
        FN("Matrix2x2::Rotate");

        T Cos = cos(a);
        T Sin = sin(a);
        return (*this) *= Matrix2x2(Cos, -Sin, Sin, Cos);
    }

    inline void ScaleX(T a)
    {
        FN("Matrix2x2::ScaleX");

        return (*this) *= Matrix2x2(a, 0, 0, 1);
    }

    inline void ScaleY(T a)
    {
        FN("Matrix2x2::ScaleY");

        return (*this) *= Matrix2x2(1, 0, 0, a);
    }

    inline void Scale(T a)
    {
        FN("Matrix2x2::Scale");

        return (*this) *= Matrix2x2(a, 0, 0, a);
    }

    inline Matrix2x2 operator*(const Matrix2x2 &other) const
    {
        FN("Matrix2x2::operator*");

        return Matrix2x2(_11 * other._11 + _12 * other._21, _11 * other._12 + _12 * other._22, _21 * other._11 + _22 * other._21, _21 * other._12 + _22 * other._22);
    }

    inline Vector2<T> operator*(const Vector2<T> &vector) const
    {
        FN("Matrix2x2::operator*");

        return Vector2<T>(_11 * vector.x + _12 * vector.y, _21 * vector.x + _22 * vector.y);
    }

    inline Matrix2x2 operator*(const T &scalar) const
    {
        FN("Matrix2x2::operator*");

        return Matrix2x2(_11 * scalar, _12 * scalar, _21 * scalar, _22 * scalar);
    }

    inline Matrix2x2 operator/(const T &scalar) const
    {
        FN("Matrix2x2::operator/");

        return Matrix2x2(_11 / scalar, _12 / scalar, _21 / scalar, _22 / scalar);
    }

    inline void operator*=(const Matrix2x2 &other)
    {
        FN("Matrix2x2::operator*=");

        Set(_11 * other._11 + _12 * other._21, _11 * other._12 + _12 * other._22, _21 * other._11 + _22 * other._21, _21 * other._12 + _22 * other._22);
    }

    inline void operator*=(const T &scalar)
    {
        FN("Matrix2x2::operator*=");

        Set(_11 * scalar, _12 * scalar, _21 * scalar, _22 * scalar);
    }

    inline void operator/=(const T &scalar)
    {
        FN("Matrix2x2::operator/=");

        Set(_11 / scalar, _12 / scalar, _21 / scalar, _22 / scalar);
    }

    inline bool operator==(const Matrix2x2 &other) const
    {
        FN("Matrix2x2::operator==");
        return _11 == other._11 && _12 == other._12 && _21 == other._21 && _22 == other._22;
    }

    inline bool operator!=(const Matrix2x2 &other) const
    {
        FN("Matrix2x2::operator!=");

        return _11 != other._11 || _12 != other._12 || _21 != other._21 || _22 != other._22;
    }

    template <typename C> inline operator Matrix2x2<C>()
    {
        FN("Matrix2x2::Matrix2x2");

        return Matrix2x2<C>((C)_11, (C)_12, (C)_21, (C)_22);
    }
};
/*----------------------------------------------------------------*/

template <typename T> inline Matrix2x2<T> operator*(float s, const Matrix2x2<T> &matrix)
{
    FN("Matrix2x2::operator*");

    return matrix * s;
}
} // namespace Core
} // namespace bow