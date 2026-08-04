#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/BowLogger.h"
#include "CoreSystems/Math/BowVector3.h"
#include <math.h>

namespace bow
{

template <typename T> class Vector4
{
  public:
    union
    {
        struct
        {
            T x, y, z, w; // Koordinaten
        };
        T a[4];
    };

    Vector4()
    {
        FN("Vector4::Vector4");

        x = y = z = 0;
        w = 1;
    }

    Vector4(T _x, T _y, T _z, T _w = 1)
    {
        FN("Vector4::Vector4");

        x = _x, y = _y, z = _z, w = _w;
    }

    template <typename C> Vector4(const Vector3<C> &other, C _w = 1)
    {
        FN("Vector4::Vector4");

        x = other.x, y = other.y, z = other.z, w = _w;
    }

    Vector4(const T *_a)
    {
        FN("Vector4::Vector4");

        x = _a[0];
        y = _a[1];
        z = _a[2];
        w = _a[3];
    }

    inline static Vector4 Lerp(Vector4 vectorA, Vector4 vectorB, T alpha)
    {
        FN("Vector4::Lerp");

        return vectorA * (1 - alpha) + vectorB * alpha;
    }

    inline void Set(T _x, T _y, T _z, T _w = 1)
    {
        FN("Vector4::Set");

        x = _x, y = _y, z = _z, w = _w;
    }

    inline T Length() const
    {
        FN("Vector4::Length");

        return sqrt(LengthSquared());
    }

    inline T LengthSquared() const
    {
        FN("Vector4::LengthSquared");

        return x * x + y * y + z * z;
    }

    inline T Angle(const Vector4 &other)
    {
        FN("Vector4::Angle");

        return acos((DotP(other)) / (Length() * other.Length()));
    }

    inline void Normalize()
    {
        FN("Vector4::Normalize");

        (*this) /= Length();
    }

    inline Vector4 Normalized()
    {
        FN("Vector4::Normalized");

        return (*this) / Length();
    }

    inline void Negate() const
    {
        FN("Vector4::Negate");

        x = -x;
        y = -y;
        z = -z;
    }

    inline Vector4 operator-() const
    {
        FN("Vector4::operator-");

        return Vector4(-x, -y, -z);
    }

    void operator+=(const Vector4 &other)
    {
        FN("Vector4::operator+=");

        x += other.x;
        y += other.y;
        z += other.z;
    }

    Vector4 operator+(const Vector4 &other) const
    {
        FN("Vector4::operator+");

        return Vector4(x + other.x, y + other.y, z + other.z);
    }

    void operator-=(const Vector4 &other)
    {
        FN("Vector4::operator-=");

        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    Vector4 operator-(const Vector4 &other) const
    {
        FN("Vector4::operator-");

        return Vector4(x - other.x, y - other.y, z - other.z);
    }

    void operator*=(T other)
    {
        FN("Vector4::operator*=");

        x *= other;
        y *= other;
        z *= other;
    }

    Vector4 operator*(T other) const
    {
        FN("Vector4::operator*");

        return Vector4(x * other, y * other, z * other);
    }

    void operator/=(T other)
    {
        FN("Vector4::operator/=");

        x /= other;
        y /= other;
        z /= other;
    }

    Vector4 operator/(T other) const
    {
        FN("Vector4::operator/");

        return Vector4(x / other, y / other, z / other);
    }

    bool operator==(const Vector4 &other) const
    {
        FN("Vector4::operator==");

        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    bool operator!=(const Vector4 &other) const
    {
        FN("Vector4::operator!=");

        return x != other.x || y != other.y || z != other.z || w != other.w;
    }

    // write a value
    inline T &operator[](int index)
    {
        FN("Vector4::operator[]");

        LOG_ASSERT(index >= 0 && index < 4, "Out of bounds of Vector4");

        return a[index];
    }

    // read a value
    inline const T &operator[](int index) const
    {
        FN("Vector4::operator[]");

        LOG_ASSERT(index >= 0 && index < 4, "Out of bounds of Vector4");

        return a[index];
    }

    template <typename C> inline operator Vector4<C>()
    {
        FN("Vector4::Vector4");

        return Vector4<C>((C)x, (C)y, (C)z, (C)w);
    }
};
/*----------------------------------------------------------------*/

// template <typename C, typename T>
// inline Vector4<T> operator *(C s, const Vector4<T>& vector)
//{
//	return vector * (T)s;
// }

// Dot product
template <typename T> inline T DotP(const Vector4<T> &v1, const Vector4<T> &v2)
{
    FN("Vector4::DotP");

    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

// Cross product calculation for a 3D Vector
template <typename T> inline Vector4<T> CrossP(const Vector4<T> &v1, const Vector4<T> &v2)
{
    FN("Vector4::CrossP");

    return Vector4<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x, 1.0f);
}

template <typename C> inline std::ostream &operator<<(std::ostream &str, const Vector4<C> &vec)
{
    FN("Vector4::operator<<");

    str << '[';
    for (size_t row = 0; row < 4; ++row)
    {
        str << vec.a[row];

        if (row != 3)
            str << ", ";
    }
    str << "]";

    return str;
}

typedef Vector4<float> ColorRGBA;
} // namespace bow
