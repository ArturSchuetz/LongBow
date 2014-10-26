#pragma once
#include "BowPrerequisites.h"

#include "BowCorePredeclares.h"
#include "BowMath.h"

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
			Camera(unsigned int width, unsigned int height, double FOV = 90);
			~Camera(){}

			// set view matrix from cam's vRight, vUp, vDirection, vPosition
			bool SetView(const Core::Vector3<double>& right, const Core::Vector3<double>& up, const Core::Vector3<double>& direction, const Core::Vector3<double>& position);

			// set look at matrix: from, at, world up
			bool SetViewLookAt(const Core::Vector3<double>& cameraPosition, const Core::Vector3<double>& lookAtPoint, const Core::Vector3<double>& worldUp);

			// set near and far clipping plane
			void SetClippingPlanes(double near, double far);

			// set field of view
			bool SetFOV(double FOV);

			// set mode for stage n, 0:=3D(perspective), 1:=2D(orthogonal)
			void SetMode(ProjectionMode mode);

			// screen to worldray, give 2 vectors for output
			//Core::Ray<double> Transform2Dto3D(const Core::Vector2<double>& screenPosition) const;

			// cast world position to screen coordinates
			//Core::Vector2<double> Transform3Dto2D(const Core::Vector3<double>& worldPosition) const;

			// get world view projection matrix or NULL
			Core::Matrix3D<double> CalculateWorldViewProjection(const Core::Matrix3D<double>* world);

		private:
			void    CalcViewProjMatrix();
			bool	CalcPerspProjMatrix();
			//void	CalcOrthoProjMatrix(double left, double right, double bottom, double top, double nearPlane, double farPlane);

			unsigned int	m_Width,	// Screenwidth (or Viewport width)
							m_Height;	// Screenheight (or Viewport height)

			double			m_Near,		// Near-Plane
							m_Far,		// Far-Plane
							m_FOV;		// Field of View
			
			ProjectionMode	m_Mode;		// persp., ortho or 2d

			Core::Matrix3D<double>	m_View,
									m_World,
									m_PerspectiveProjection,
									m_OrthographicProjection,
									m_ViewProjection;
		};
	}
}
