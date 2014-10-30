#include "BowCamera.h"
#include "BowLogger.h"

namespace Bow
{
	namespace Renderer
	{
		Camera::Camera(unsigned int width, unsigned int height)
		{
			m_Width = width;
			m_Height = height;
			m_FOV = 90.0f * M_PI / 360.0f;
			m_Mode = ProjectionMode::Perspective;

			m_Near = 0.1f;
			m_Far = 1000.0f;

			m_View.SetIdentity();
			CalcPerspProjMatrix();
		}
		/*----------------------------------------------------------------*/

		bool Camera::SetViewLookAt(	const Core::Vector3<double>& cameraPosition,
									const Core::Vector3<double>& lookAtPoint,
									const Core::Vector3<double>& worldUp)
		{
			Core::Vector3<double> vcDir, vcTemp, vcUp;

			vcDir = lookAtPoint - cameraPosition;
			vcDir.Normalize();

			// calculate up vector
			double fDot = worldUp.DotP(vcDir);

			vcTemp = vcDir * fDot;
			vcUp = worldUp - vcTemp;
			double fL = vcUp.Length();

			// if length too small take normal y axis as up vector
			if (fL < 1e-6f)
			{
				Core::Vector3<double> vcY;
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
					if (fL < 1e-6f) return  false;
				}
			}

			vcUp /= fL;

			// build right vector using cross product
			Core::Vector3<double> vcRight(vcUp.CrossP(vcDir));

			// build final matrix and set for device
			return SetView(vcRight, vcUp, vcDir, cameraPosition);
		}
		/*----------------------------------------------------------------*/

		bool Camera::SetView(const Core::Vector3<double>& right,
			const Core::Vector3<double>& up,
			const Core::Vector3<double>& direction,
			const Core::Vector3<double>& position)
		{
			memset(&m_View, 0, sizeof(Core::Matrix3D<double>));

			m_View._11 = right.x;
			m_View._12 = right.y;
			m_View._13 = right.z;
			m_View._14 = -(right.DotP(position));

			m_View._21 = up.x;
			m_View._22 = up.y;
			m_View._23 = up.z;
			m_View._24 = -(up.DotP(position));

			m_View._34 = direction.x;
			m_View._32 = direction.y;
			m_View._33 = direction.z;
			m_View._34 = -(direction.DotP(position));

			m_View._44 = 1.0f;

			m_ViewProjection = m_PerspectiveProjection * m_View;
			return  true;
		}
		/*----------------------------------------------------------------*/

		void Camera::SetResolution(unsigned int width, unsigned int height)
		{
			if (m_Width != width || m_Height != height)
			{
				m_Width = width;
				m_Height = height;

				dirty = true;
			}
		}
		/*----------------------------------------------------------------*/

		void Camera::SetClippingPlanes(double near, double far)
		{
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

		void Camera::SetFOV(double FOV)
		{
			FOV *= M_PI / 360.0f; // degree to radian
			if (m_FOV != FOV)
			{
				m_FOV = FOV;

				dirty = true;
			}
		}

		void Camera::SetMode(ProjectionMode mode)
		{
			if (m_Mode != mode)
			{
				m_Mode = mode;

				dirty = true;
			}
		}
		/*----------------------------------------------------------------*/
		
		Core::Ray<double> Camera::Transform2Dto3D(const unsigned int screenX, const unsigned int screenY)
		{
			if (dirty)
				CalcPerspProjMatrix();

			Core::Matrix3D<double> InvView;
			Core::Vector3<double> vcS;

			// resize to viewportspace [-1,1] -> projection
			vcS.x = ((((double)screenX * 2.0f) / m_Width) - 1.0f) / m_PerspectiveProjection._11;
			vcS.y = -((((double)screenY * 2.0f) / m_Height) - 1.0f) / m_PerspectiveProjection._22;
			vcS.z = 1.0f;

			// invert view matrix
			InvView = m_View.Inverse();

			// ray from screen to worldspace
			Core::Ray<double> result;
			result.Direction.x = (vcS.x * InvView._11) + (vcS.y * InvView._12) + (vcS.z * InvView._13);
			result.Direction.y = (vcS.x * InvView._21) + (vcS.y * InvView._22) + (vcS.z * InvView._23);
			result.Direction.z = (vcS.x * InvView._31) + (vcS.y * InvView._32) + (vcS.z * InvView._33);

			// inverse translation.
			result.Origin.x = InvView._14;
			result.Origin.y = InvView._24;
			result.Origin.z = InvView._34;

			// normalize
			result.Direction.Normalize();
			return result;
		}
		/*----------------------------------------------------------------*/

		Core::Vector2<double> Camera::Transform3Dto2D(const Core::Vector3<double> &worldPosition)
		{
			if (dirty)
				CalcPerspProjMatrix();

			Core::Vector2<double> pt;
			double fClip_x, fClip_y;
			double fXp, fYp, fWp;

			fClip_x = (double)(m_Width >> 1);
			fClip_y = (double)(m_Height >> 1);

			fXp = (m_ViewProjection._11*worldPosition.x) + (m_ViewProjection._12*worldPosition.y) + (m_ViewProjection._13*worldPosition.z) + m_ViewProjection._14;
			fYp = (m_ViewProjection._21*worldPosition.x) + (m_ViewProjection._22*worldPosition.y) + (m_ViewProjection._23*worldPosition.z) + m_ViewProjection._24;
			fWp = (m_ViewProjection._41*worldPosition.x) + (m_ViewProjection._42*worldPosition.y) + (m_ViewProjection._43*worldPosition.z) + m_ViewProjection._44;

			double fWpInv = 1.0f / fWp;

			// transform from [-1,1] to actual viewport dimensions
			pt.x = ((1.0f + (fXp * fWpInv)) * fClip_x);
			pt.y = ((1.0f + (fYp * fWpInv)) * fClip_y);

			return pt;
		}
		/*----------------------------------------------------------------*/

		Core::Matrix3D<double> Camera::CalculateWorldViewProjection()
		{
			if (dirty)
				CalcPerspProjMatrix();

			return m_ViewProjection;
		}
		
		Core::Matrix3D<double> Camera::CalculateWorldViewProjection(const Core::Matrix3D<double>& world)
		{
			if (dirty)
				CalcPerspProjMatrix();

			return m_ViewProjection * world.Inverse();
		}
		/*----------------------------------------------------------------*/
		
		bool Camera::CalcPerspProjMatrix()
		{
			if (fabs(m_Far - m_Near) < 0.01f)
				return false;

			double sinFOV2 = (double)sinf(m_FOV / 2);

			if (fabs(sinFOV2) < 0.01f)
				return false;

			double cosFOV2 = (double)cosf(m_FOV / 2);

			double Aspect = (double)m_Height / (double)m_Width;
			double w = Aspect * (cosFOV2 / sinFOV2);
			double h = 1.0f  * (cosFOV2 / sinFOV2);
			double Q = m_Far / (m_Far - m_Near);

			memset(&m_PerspectiveProjection, 0, sizeof(Core::Matrix3D<double>));
			m_PerspectiveProjection._11 = w;
			m_PerspectiveProjection._22 = h;
			m_PerspectiveProjection._33 = Q;
			m_PerspectiveProjection._43 = 1.0f;
			m_PerspectiveProjection._34 = -Q*m_Near;

			m_ViewProjection = m_PerspectiveProjection * m_View;

			dirty = false;
			return true;
		}
		/*----------------------------------------------------------------*/
	}
}