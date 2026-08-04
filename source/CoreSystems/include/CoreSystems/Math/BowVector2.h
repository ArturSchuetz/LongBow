#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/BowLogger.h"
#include <math.h>

namespace bow
{

template <typename T> class Vector2
{
  public:
    union
    {
        struct
        {
            T x, y; // Koordinaten
        };
        T a[2];
    };

    Vector2()
    {
        FN("Vector2::Vector2");

        x = y = 0;
    }

    Vector2(T _x, T _y)
    {
        FN("Vector2::Vector2");

        x = _x, y = _y;
    }

    Vector2(const T *_a)
    {
        FN("Vector2::Vector2");

        x = _a[0];
        y = _a[1];
    }

    inline static Vector2 Lerp(Vector2 vectorA, Vector2 vectorB, T alpha)
    {
        FN("Vector2::Lerp");

        return vectorA * (1 - alpha) + vectorB * alpha;
    }

    inline void Set(T _x, T _y)
    {
        FN("Vector2::Set");

        x = _x, y = _y;
    }

    inline T Length() const
    {
        FN("Vector2::Length");

        return sqrt(LengthSquared());
    }

    inline T LengthSquared() const
    {
        FN("Vector2::LengthSquared");

        return x * x + y * y;
    }

    inline T Angle(const Vector2 &other)
    {
        FN("Vector2::Angle");

        return acos((DotP(*this, other)) / (Length() * other.Length()));
    }

    inline void Normalize()
    {
        FN("Vector2::Normalize");

        (*this) /= Length();
    }

    inline Vector2 Normalized()
    {
        FN("Vector2::Normalized");

        return (*this) / Length();
    }

    inline void Negate()
    {
        FN("Vector2::Negate");

        x = -x;
        y = -y;
    }

    inline Vector2 operator-() const
    {
        FN("Vector2::operator-");

        return Vector2(-x, -y);
    }

    inline void operator+=(const Vector2 &other)
    {
        FN("Vector2::operator+=");

        x += other.x;
        y += other.y;
    }

    inline Vector2 operator+(const Vector2 &other) const
    {
        FN("Vector2::operator+");

        return Vector2(x + other.x, y + other.y);
    }

    inline void operator-=(const Vector2 &other)
    {
        FN("Vector2::operator-=");

        x -= other.x;
        y -= other.y;
    }

    inline Vector2 operator-(const Vector2 &other) const
    {
        FN("Vector2::operator-");

        return Vector2(x - other.x, y - other.y);
    }

    inline void operator*=(T other)
    {
        FN("Vector2::operator*=");

        x *= other;
        y *= other;
    }

    inline Vector2 operator*(T other) const
    {
        FN("Vector2::operator*");

        return Vector2(x * other, y * other);
    }

    inline void operator/=(T other)
    {
        FN("Vector2::operator/=");

        x /= other;
        y /= other;
    }

    inline Vector2 operator/(T other) const
    {
        FN("Vector2::operator/");

        return Vector2(x / other, y / other);
    }

    inline bool operator==(const Vector2 &other) const
    {
        FN("Vector2::operator==");

        return x == other.x && y == other.y;
    }

    inline bool operator!=(const Vector2 &other) const
    {
        FN("Vector2::operator!=");

        return x != other.x || y != other.y;
    }

    // write a value
    inline T &operator[](int index)
    {
        FN("Vector2::operator[]");

        LOG_ASSERT(index >= 0 && index < 2, "Out of bounds of Vector2");

        return a[index];
    }

    // read a value
    inline const T &operator[](int index) const
    {
        FN("Vector2::operator[]");

        LOG_ASSERT(index >= 0 && index < 2, "Out of bounds of Vector2");

        return a[index];
    }

    template <typename C> inline operator Vector2<C>()
    {
        FN("Vector2::Vector2");

        return Vector2<C>((C)x, (C)y);
    }
};
/*----------------------------------------------------------------*/

template <typename C, typename T> inline Vector2<T> operator*(C s, const Vector2<T> &vector)
{
    FN("Vector2::operator*");

    return vector * (T)s;
}

// Dot product
template <typename T> inline T DotP(const Vector2<T> &v1, const Vector2<T> &v2)
{
    FN("Vector2::DotP");

    return v1.x * v2.x + v1.y * v2.y;
}

template <typename C> inline std::ostream &operator<<(std::ostream &str, const Vector2<C> &vec)
{
    FN("Vector2::operator<<");

    str << '[';
    for (size_t row = 0; row < 2; ++row)
    {
        str << vec.a[row];

        if (row != 1)
            str << ", ";
    }
    str << "]";

    return str;
}
} // namespace bow
