#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Math/BowMatrix3x3.h"
#include "CoreSystems/Math/BowVector3.h"
#include <math.h>

namespace bow
{

template <typename T> class Transform
{
  public:
    explicit Transform()
    {
        FN("Transform::Transform");

        SetIdentity();
    }

    explicit Transform(const Vector3<T> &Position, const Matrix3x3<T> &Rotation = Matrix3x3<T>::Identity(), const Vector3<T> &Scale = Vector3<T>::One())
    {
        FN("Transform::Transform");

        m_position = Position;
        m_rotation = Rotation;
        m_scale = Scale;
    }

    void SetIdentity()
    {
        FN("Transform::SetIdentity");

        m_position = Vector3<T>::Zero();
        m_rotation = Matrix3x3<T>::Identity();
        m_scale = Vector3<T>::One();
    }

    Vector3<T> GetPosition() const
    {
        FN("Transform::GetPosition");

        return m_position;
    }
    void SetPosition(Vector3<T> position)
    {
        FN("Transform::SetPosition");

        m_position = position;
    }

    Matrix3x3<T> GetRotation() const
    {
        FN("Transform::GetRotation");

        return m_rotation;
    }
    void SetRotation(Matrix3x3<T> rotation)
    {
        FN("Transform::SetRotation");

        m_rotation = rotation;
    }

    Vector3<T> GetScale() const
    {
        FN("Transform::GetScale");

        return m_scale;
    }
    void SetScale(Vector3<T> scale)
    {
        FN("Transform::SetScale");

        m_scale = scale;
    }

  private:
    Vector3<T> m_position;
    Matrix3x3<T> m_rotation;
    Vector3<T> m_scale;
};
/*----------------------------------------------------------------*/

} // namespace bow
