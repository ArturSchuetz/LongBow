#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "BowMath.h"

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

namespace bow {

	enum struct ProjectionMode : char
	{
		Perspective,
		Orthogonal
	};

	class Camera
	{
	public:
		Camera(unsigned int width, unsigned int height);
		Camera(const Vector3<double>& cameraPosition, const Vector3<double>& lookAtPoint, const Vector3<double>& worldUp, unsigned int width, unsigned int height);
		~Camera(){}

		// set look at matrix: from, at, world up
		bool SetViewLookAt(const Vector3<double>& cameraPosition, const Vector3<double>& lookAtPoint, const Vector3<double>& worldUp);

		// set view matrix from cam's vRight, vUp, vDirection, vPosition
		bool SetView(const Vector3<double>& right, const Vector3<double>& up, const Vector3<double>& direction, const Vector3<double>& position);

		// set width and height for projection
		void SetResolution(unsigned int width, unsigned int height);

		unsigned int GetXResolution() { return m_Width; }
		unsigned int GetYResolution() { return m_Height; }

		// set near and far clipping plane
		void SetClippingPlanes(double near, double far);

		// set field of view
		void SetFOV(float FOV);

		// screen to worldray, give ray for output
		Ray<double> Transform2Dto3D(const unsigned int screenX, const unsigned int screenY);

		// cast world position to screen coordinates
		Vector3<double> Transform3Dto2D(const Vector3<double>& worldPosition);
			
		// get world view projection matrix or NULL
		Matrix3D<double> CalculateView();
		Matrix3D<double> CalculateWorldView(const Matrix3D<double>& world);
		Matrix4x4<double> CalculateViewProjection();
		Matrix4x4<double> CalculateWorldViewProjection(const Matrix3D<double>& world);
		Matrix4x4<double> CalculateProjection();
		Frustum<double> CalculateFrustum(const Matrix3D<double>& world);

	private:
		bool CalcPerspProjMatrix();

		unsigned int	m_Width,	// Screenwidth (or Viewport width)
						m_Height;	// Screenheight (or Viewport height)

		double			m_Near,		// Near-Plane
						m_Far;		// Far-Plane
		float			m_FOV;		// Field of View
			
		ProjectionMode	m_Mode;		// persp., ortho or 2d

		Matrix3D<double>	m_View;
		Matrix4x4<double>	m_Projection,
								m_ViewProjection;
		Frustum<double>	m_Frustum;
		bool dirty;
	};
}

