#pragma once
#include "BowPrerequisites.h"

#include "BowCorePredeclares.h"
#include "BowMath.h"

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

namespace Bow {
	namespace Renderer {

		enum struct ProjectionMode : char
		{
			Perspective,
			Orthogonal
		};

		class Camera
		{
		public:
			Camera(unsigned int width, unsigned int height);
			~Camera(){}

			// set look at matrix: from, at, world up
			bool SetViewLookAt(const Core::Vector3<double>& cameraPosition, const Core::Vector3<double>& lookAtPoint, const Core::Vector3<double>& worldUp);

			// set view matrix from cam's vRight, vUp, vDirection, vPosition
			bool SetView(const Core::Vector3<double>& right, const Core::Vector3<double>& up, const Core::Vector3<double>& direction, const Core::Vector3<double>& position);

			// set width and height for projection
			void SetResolution(unsigned int width, unsigned int height);

			// set near and far clipping plane
			void SetClippingPlanes(double near, double far);

			// set field of view
			void SetFOV(float FOV);

			// set mode for stage n, 0:=3D(perspective), 1:=2D(orthogonal)
			void SetMode(ProjectionMode mode);

			// screen to worldray, give ray for output
			Core::Ray<double> Transform2Dto3D(const unsigned int screenX, const unsigned int screenY);

			// cast world position to screen coordinates
			Core::Vector2<double> Transform3Dto2D(const Core::Vector3<double>& worldPosition);

			// get world view projection matrix or NULL
			Core::Matrix3D<double> CalculateWorldViewProjection();
			Core::Matrix3D<double> CalculateWorldViewProjection(const Core::Matrix3D<double>& world);

		private:
			bool	CalcPerspProjMatrix();

			unsigned int	m_Width,	// Screenwidth (or Viewport width)
							m_Height;	// Screenheight (or Viewport height)

			double			m_Near,		// Near-Plane
							m_Far;		// Far-Plane
			float			m_FOV;		// Field of View
			
			ProjectionMode	m_Mode;		// persp., ortho or 2d

			Core::Matrix3D<double>	m_View,
									m_PerspectiveProjection,
									m_ViewProjection;

			bool dirty;
		};
	}
}
