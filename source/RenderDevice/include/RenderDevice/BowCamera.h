#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <CoreSystems/BowMath.h>

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

namespace bow
{

enum struct ProjectionMode : char
{
    Perspective,
    Orthogonal
};

class RENDERDEVICE_API Camera
{
  public:
    Camera(uint32_t width, uint32_t height, ProjectionMode mode = ProjectionMode::Perspective);
    Camera(const Vector3<float> &cameraPosition, const Vector3<float> &lookAtPoint, const Vector3<float> &worldUp, uint32_t width, uint32_t height, ProjectionMode mode = ProjectionMode::Perspective);
    ~Camera() {}

    Vector3<float> GetPosition() { return Vector3<float>(m_View._14, m_View._24, m_View._34); }
    float GetVerticalFOVinRadian();
    ProjectionMode GetProjectionMode() { return m_Mode; }

    uint32_t GetXResolution() const { return m_Width; }
    uint32_t GetYResolution() const { return m_Height; }

    float GetNearClippingDistance() const { return m_Near; }
    float GetFarClippingDistance() const { return m_Far; }

    // set look at matrix: from, at, world up
    bool SetViewLookAt(const Vector3<float> &cameraPosition, const Vector3<float> &lookAtPoint, const Vector3<float> &worldUp);

    // set view matrix from cam's vRight, vUp, vDirection, vPosition
    bool SetView(const Vector3<float> &right, const Vector3<float> &up, const Vector3<float> &direction, const Vector3<float> &position);

    // set view matrix directly (transforms from world to view coordinate
    // system)
    bool SetView(const Matrix4x4<float> &view);

    // set width and height for projection
    void SetResolution(uint32_t width, uint32_t height);

    // set near and far clipping plane
    void SetClippingPlanes(float near, float far);

    // set field of view
    void SetFOV(float FOV);
    void SetFOVasRadian(float FOV);

    // screen to worldray, give ray for output
    Ray<float> Transform2Dto3D(const uint32_t screenX, const uint32_t screenY);

    // cast world position to screen coordinates
    Vector2<float> Transform3Dto2D(const Vector3<float> &worldPosition);

    // get world view projection matrix or NULL
    Matrix3D<float> CalculateView();
    Matrix3D<float> CalculateWorldView(const Matrix3D<float> &world);
    Matrix4x4<float> CalculateViewProjection();
    Matrix4x4<float> CalculateWorldViewProjection(const Matrix3D<float> &world);
    Matrix4x4<float> CalculateProjection();

#ifndef _WIN32
    Frustum<float> CalculateFrustum(const Matrix3D<float> &world);
#endif

  private:
    bool CalcPerspProjMatrix();
    bool CalcOrthProjMatrix();

    uint32_t m_Width, // Screenwidth (or Viewport width)
        m_Height;     // Screenheight (or Viewport height)

    float m_Near, // Near-Plane
        m_Far;    // Far-Plane
    float m_FOV;  // Field of View

    ProjectionMode m_Mode; // persp., ortho or 2d

    Matrix3D<float> m_View;
    Matrix4x4<float> m_Projection, m_ViewProjection;

#ifndef _WIN32
    Frustum<float> m_Frustum;
#endif
    bool dirty;
};
} // namespace bow
