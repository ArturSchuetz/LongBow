#include "BowCamera.h"
#include "BowLogger.h"

namespace Bow
{
	namespace Renderer
	{
		const Core::Matrix3D<double> negativeZ = Core::Matrix3D<double>(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0);

		Camera::Camera(unsigned int width, unsigned int height)
		{
			m_Width = width;
			m_Height = height;
			m_FOV = 90.0f * M_PI / 360.0f;
			m_Mode = ProjectionMode::Perspective;

			m_Near = 0.5f;
			m_Far = 500.0f;

			m_View.SetIdentity();
			CalcPerspProjMatrix();
		}
		/*----------------------------------------------------------------*/

		Camera::Camera(const Core::Vector3<double>& cameraPosition, const Core::Vector3<double>& lookAtPoint, const Core::Vector3<double>& worldUp, unsigned int width, unsigned int height) : Camera(width, height)
		{
			SetViewLookAt(cameraPosition, lookAtPoint, worldUp);
		}

		bool Camera::SetViewLookAt(	const Core::Vector3<double>& cameraPosition,
									const Core::Vector3<double>& lookAtPoint,
									const Core::Vector3<double>& worldUp)
		{
			Core::Vector3<double> vcDir, vcTemp, vcUp;

			vcDir = lookAtPoint - cameraPosition;
			vcDir.Normalize();

			// calculate up vector
			double fDot = DotP(worldUp, vcDir);

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
			Core::Vector3<double> vcRight(Bow::Core::CrossP(vcDir, vcUp));

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

			m_ViewProjection = m_Projection * m_View;

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

		void Camera::SetFOV(float FOV)
		{
			FOV *= M_PI / 360.0f; // degree to radian
			if (m_FOV != FOV)
			{
				m_FOV = FOV;

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
			vcS.x = ((((double)screenX * 2.0f) / m_Width) - 1.0f) / m_Projection._11;
			vcS.y = -((((double)screenY * 2.0f) / m_Height) - 1.0f) / m_Projection._22;
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

			fClip_x = (double)(m_Width >> 1);
			fClip_y = (double)(m_Height >> 1);

			Core::Vector4<double> out = m_ViewProjection * worldPosition;

			double fWpInv = 1.0 / out.z;

			// transform from [-1,1] to actual viewport dimensions
			pt.x = ((1.0f + (out.x * fWpInv)) * fClip_x);
			pt.y = ((1.0f + (out.y * fWpInv)) * fClip_y);

			return pt;
		}
		/*----------------------------------------------------------------*/

		Core::Matrix3D<double> Camera::CalculateView()
		{
			return m_View;
		}

		Core::Matrix3D<double> Camera::CalculateWorldView(const Core::Matrix3D<double>& world)
		{
			return m_View * world;
		}

		Core::Matrix4x4<double> Camera::CalculateProjection()
		{
			if (dirty)
				CalcPerspProjMatrix();

			return m_Projection;
		}

		Core::Matrix4x4<double> Camera::CalculateViewProjection()
		{
			if (dirty)
				CalcPerspProjMatrix();

			return m_ViewProjection;
		}

		Core::Matrix4x4<double> Camera::CalculateWorldViewProjection(const Core::Matrix3D<double>& world)
		{
			if (dirty)
				CalcPerspProjMatrix();

			return m_ViewProjection * world;
		}

		Core::Frustum<double> Camera::CalculateFrustum(const Core::Matrix3D<double>& world)
		{
			if (dirty)
				CalcPerspProjMatrix();

			return Core::Frustum<double>(m_ViewProjection * world);
		}
		/*----------------------------------------------------------------*/
		
		bool Camera::CalcPerspProjMatrix()
		{
			if (fabs(m_Far - m_Near) < 0.01f)
				return false;

			float sinFOV2 = sinf(m_FOV / 2.0f);

			if (fabs(sinFOV2) < 0.01f)
				return false;

			float cosFOV2 = cosf(m_FOV / 2.0f);

			float Aspect = (float)m_Height / (float)m_Width;
			float w = Aspect * (cosFOV2 / sinFOV2);
			float h = 1.0f  * (cosFOV2 / sinFOV2);
			double Q = m_Far / (m_Far - m_Near);

			memset(&m_Projection, 0, sizeof(Core::Matrix3D<double>));
			// Old
			m_Projection._11 = w; // Wieso das Minus?
			m_Projection._22 = h;
			m_Projection._33 = Q;
			m_Projection._43 = 1.0f;
			m_Projection._34 = -Q*m_Near;

			m_ViewProjection = m_Projection * m_View;

			dirty = false;
			return true;
		}
		/*----------------------------------------------------------------*/

	}
}