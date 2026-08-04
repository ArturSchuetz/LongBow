#include <RenderDevice/BowCamera.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace bow
{
const Matrix3D<float> negativeX = Matrix3D<float>(-1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
const Matrix3D<float> negativeY = Matrix3D<float>(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0);
const Matrix3D<float> negativeZ = Matrix3D<float>(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0);

Camera::Camera(uint32_t width, uint32_t height, ProjectionMode mode)
{
    FN("Camera::Camera");
    OPTICK_EVENT();

    m_Width = width;
    m_Height = height;
    m_FOV = 45.0f * M_PI / 180.0f;
    m_Mode = ProjectionMode::Perspective;

    m_Near = 0.1f;
    m_Far = 1000.0f;
    m_Mode = mode;

    m_View.SetIdentity();
    CalcPerspProjMatrix();
}
/*----------------------------------------------------------------*/

Camera::Camera(const Vector3<float> &cameraPosition, const Vector3<float> &lookAtPoint, const Vector3<float> &worldUp, uint32_t width, uint32_t height, ProjectionMode mode) : Camera(width, height, mode)
{
    FN("Camera::Camera");
    OPTICK_EVENT();

    SetViewLookAt(cameraPosition, lookAtPoint, worldUp);
}

bool Camera::SetViewLookAt(const Vector3<float> &cameraPosition, const Vector3<float> &lookAtPoint, const Vector3<float> &worldUp)
{
    FN("Camera::SetViewLookAt");
    OPTICK_EVENT();

    Vector3<float> vcDir, vcTemp, vcUp;

    vcDir = lookAtPoint - cameraPosition;
    vcDir.Normalize();

    // calculate up vector
    float fDot = DotP(worldUp, vcDir);

    vcTemp = vcDir * fDot;
    vcUp = worldUp - vcTemp;
    float fL = vcUp.Length();

    // if length too small take normal y axis as up vector
    if (fL < 1e-6f)
    {
        Vector3<float> vcY;
        vcY.Set(0.0f, 1.0f, 0.0f);

        vcTemp = vcDir * vcDir.y;
        vcUp = vcY - vcTemp;

        fL = vcUp.Length();

        // if still too small take z axis as up vector
        if (fL < 1e-6f)
        {
            vcY.Set(0.0f, 0.0f, 1.0f);

            vcTemp = vcDir * vcDir.z;
            vcUp = vcY - vcTemp;

            // if still too small we are lost
            fL = vcUp.Length();
            if (fL < 1e-6f)
                return false;
        }
    }

    vcUp /= fL;

    // build right vector using cross product
    Vector3<float> vcRight(CrossP(vcDir, vcUp));

    // build final matrix and set for device
    return SetView(vcRight, vcUp, vcDir, cameraPosition);
}
/*----------------------------------------------------------------*/

bool Camera::SetView(const Vector3<float> &right, const Vector3<float> &up, const Vector3<float> &direction, const Vector3<float> &position)
{
    FN("Camera::SetView");
    OPTICK_EVENT();

    memset(&m_View, 0, sizeof(Matrix3D<float>));

    m_View._11 = right.x;
    m_View._12 = right.y;
    m_View._13 = right.z;
    m_View._14 = -(DotP(right, position));

    m_View._21 = up.x;
    m_View._22 = up.y;
    m_View._23 = up.z;
    m_View._24 = -(DotP(up, position));

    m_View._31 = direction.x;
    m_View._32 = direction.y;
    m_View._33 = direction.z;
    m_View._34 = -(DotP(direction, position));

    m_View._44 = 1.0f;

    m_ViewProjection = m_Projection * (bow::Matrix4x4<float>)m_View;

    return true;
}
/*----------------------------------------------------------------*/
bool Camera::SetView(const bow::Matrix4x4<float> &view)
{
    FN("Camera::SetView");
    OPTICK_EVENT();

    m_View = view;

    m_ViewProjection = m_Projection * (bow::Matrix4x4<float>)m_View;

    return true;
}

/*----------------------------------------------------------------*/

void Camera::SetResolution(uint32_t width, uint32_t height)
{
    FN("Camera::SetResolution");
    OPTICK_EVENT();

    if (m_Width != width || m_Height != height)
    {
        m_Width = width;
        m_Height = height;

        dirty = true;
    }
}
/*----------------------------------------------------------------*/

void Camera::SetClippingPlanes(float near, float far)
{
    FN("Camera::SetClippingPlanes");
    OPTICK_EVENT();

    if (m_Near != near || m_Far != far)
    {
        m_Near = near;
        m_Far = far;

        if (m_Far <= 0.0f)
            m_Near = 0.01f;

        if (m_Far <= 1.0f)
            m_Far = 1.00f;

        if (m_Near >= m_Far)
        {
            m_Near = m_Far;
            m_Far = m_Near + 1.0f;
        }

        dirty = true;
    }
}
/*----------------------------------------------------------------*/

void Camera::SetFOV(float FOV)
{
    FN("Camera::SetFOV");
    OPTICK_EVENT();

    FOV *= M_PI / 180.0f;
    if (m_FOV != FOV)
    {
        m_FOV = FOV;

        dirty = true;
    }
}

void Camera::SetFOVasRadian(float FOV)
{
    FN("Camera::SetFOVasRadian");
    OPTICK_EVENT();

    FOV *= M_PI / 180.0f;
    if (m_FOV != FOV)
    {
        m_FOV = FOV;

        dirty = true;
    }
}

float Camera::GetVerticalFOVinRadian()
{
    FN("Camera::GetVerticalFOVinRadian");
    OPTICK_EVENT();

    return m_FOV;
}

/*----------------------------------------------------------------*/

Ray<float> Camera::Transform2Dto3D(const uint32_t screenX, const uint32_t screenY)
{
    FN("Camera::Transform2Dto3D");
    OPTICK_EVENT();

    if (dirty)
    {
        if (m_Mode == ProjectionMode::Perspective)
            CalcPerspProjMatrix();
        else
            CalcOrthProjMatrix();
    }

    Matrix3D<float> InvView;
    Vector3<float> vcS;

    // resize to viewportspace [-1,1] -> projection
    vcS.x = ((((float)screenX * 2.0f) / m_Width) - 1.0f) / m_Projection.a[0];
    vcS.y = -((((float)screenY * 2.0f) / m_Height) - 1.0f) / m_Projection.a[5];
    vcS.z = 1.0f;

    // invert view matrix
    InvView = m_View.Inverse();

    // ray from screen to worldspace
    Ray<float> result;
    result.direction.x = (vcS.x * InvView._11) + (vcS.y * InvView._12) + (vcS.z * InvView._13);
    result.direction.y = (vcS.x * InvView._21) + (vcS.y * InvView._22) + (vcS.z * InvView._23);
    result.direction.z = (vcS.x * InvView._31) + (vcS.y * InvView._32) + (vcS.z * InvView._33);

    // inverse translation.
    result.origin.x = InvView._14;
    result.origin.y = InvView._24;
    result.origin.z = InvView._34;

    // normalize
    result.direction.Normalize();
    return result;
}
/*----------------------------------------------------------------*/

Vector2<float> Camera::Transform3Dto2D(const Vector3<float> &worldPosition)
{
    FN("Camera::Transform3Dto2D");
    OPTICK_EVENT();

    if (dirty)
    {
        if (m_Mode == ProjectionMode::Perspective)
            CalcPerspProjMatrix();
        else
            CalcOrthProjMatrix();
    }

    Vector2<float> pt;
    float fClip_x, fClip_y;

    fClip_x = (float)(m_Width >> 1);
    fClip_y = (float)(m_Height >> 1);

    Vector4<float> out = m_ViewProjection * Vector4<float>(worldPosition.x, worldPosition.y, worldPosition.z, 1.0);

    float fWpInv = 1.0 / out.a[3];

    // transform from [-1,1] to actual viewport dimensions
    pt.x = ((1.0f + (out.a[0] * fWpInv)) * fClip_x);
    pt.y = ((1.0f + (out.a[1] * fWpInv)) * fClip_y);

    return pt;
}
/*----------------------------------------------------------------*/

Matrix3D<float> Camera::CalculateView()
{
    FN("Camera::CalculateView");
    OPTICK_EVENT();

    return m_View;
}

Matrix3D<float> Camera::CalculateWorldView(const Matrix3D<float> &world)
{
    FN("Camera::CalculateWorldView");
    OPTICK_EVENT();

    return m_View * world;
}

Matrix4x4<float> Camera::CalculateProjection()
{
    FN("Camera::CalculateProjection");
    OPTICK_EVENT();

    if (dirty)
    {
        if (m_Mode == ProjectionMode::Perspective)
            CalcPerspProjMatrix();
        else
            CalcOrthProjMatrix();
    }

    return m_Projection;
}

Matrix4x4<float> Camera::CalculateViewProjection()
{
    FN("Camera::CalculateViewProjection");
    OPTICK_EVENT();

    if (dirty)
    {
        if (m_Mode == ProjectionMode::Perspective)
            CalcPerspProjMatrix();
        else
            CalcOrthProjMatrix();
    }

    return m_ViewProjection;
}

Matrix4x4<float> Camera::CalculateWorldViewProjection(const Matrix3D<float> &world)
{
    FN("Camera::CalculateWorldViewProjection");
    OPTICK_EVENT();

    if (dirty)
    {
        if (m_Mode == ProjectionMode::Perspective)
            CalcPerspProjMatrix();
        else
            CalcOrthProjMatrix();
    }

    Matrix4x4<float> _world(world);
    return m_ViewProjection * _world;
}

#ifndef _WIN32
Frustum<float> Camera::CalculateFrustum(const Matrix3D<float> &world)
{
    FN("Camera::CalculateFrustum");
    OPTICK_EVENT();

    if (dirty)
        CalcPerspProjMatrix();

    cv::Matx<float, 4, 4> _world(world);
    return Frustum<float>(m_ViewProjection * _world);
}
#endif
/*----------------------------------------------------------------*/

bool Camera::CalcPerspProjMatrix()
{
    FN("Camera::CalcPerspProjMatrix");
    OPTICK_EVENT();

    if (fabs(m_Far - m_Near) < 0.01f)
        return false;

    float sinFOV2 = sinf(m_FOV / 2.0f);

    if (fabs(sinFOV2) < 0.01f)
        return false;

    float cosFOV2 = cosf(m_FOV / 2.0f);

    float Aspect = (float)m_Height / (float)m_Width;
    float w = Aspect * (cosFOV2 / sinFOV2);
    float h = 1.0f * (cosFOV2 / sinFOV2);
    float Q = m_Far / (m_Far - m_Near);

    memset(&m_Projection, 0, sizeof(Matrix3D<float>));

    m_Projection.a[0] = w;
    m_Projection.a[5] = h;
    m_Projection.a[10] = Q;
    m_Projection.a[11] = -Q * m_Near;
    m_Projection.a[14] = 1.0f;

    m_ViewProjection = m_Projection * (Matrix4x4<float>)m_View;

    dirty = false;
    return true;
}

bool Camera::CalcOrthProjMatrix()
{
    FN("Camera::CalcOrthProjMatrix");
    OPTICK_EVENT();

    if (fabs(m_Far - m_Near) < 0.01f)
        return false;

    memset(&m_Projection, 0, sizeof(Matrix3D<float>));

    m_Projection.a[0] = 2.0f / (float)m_Width;
    m_Projection.a[5] = 2.0f / (float)m_Height;
    m_Projection.a[10] = 2.0f / ((float)m_Far - (float)m_Near);

    m_Projection.a[11] = -(((float)m_Far + (float)m_Near) / ((float)m_Far - (float)m_Near));

    m_Projection.a[15] = 1.0f;

    m_ViewProjection = m_Projection * (Matrix4x4<float>)m_View;

    dirty = false;
    return true;
}

} // namespace bow
