/**
 * @file BowTransform.h
 * @brief Declarations for BowTransform.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <math.h>
#include "BowVector3.h"
#include "BowMatrix3x3.h"

namespace bow {

	template <typename T> class Transform
	{
	public:
		explicit Transform() 
		{
			SetIdentity();
		}

		explicit Transform(const Vector3<T>& Position, const Matrix3x3<T>& Rotation = Matrix3x3<T>::Identity(), const Vector3<T>& Scale = Vector3<T>::One())
		{
			m_position	= Position;
			m_rotation	= Rotation;
			m_scale		= Scale;
		}

		void SetIdentity()
		{
			m_position	= Vector3<T>::Zero();
			m_rotation	= Matrix3x3<T>::Identity();
			m_scale		= Vector3<T>::One();
		}

		Vector3<T> GetPosition(void) const { return m_position; }
		void SetPosition(Vector3<T> position) { m_position = position; }

		Matrix3x3<T> GetRotation(void) const { return m_rotation; }
		void SetRotation(Matrix3x3<T> rotation) { m_rotation = rotation; }

		Vector3<T> GetScale(void) const { return m_scale; }
		void SetScale(Vector3<T> scale) { m_scale = scale; }

	private:
		Vector3<T>		m_position;
		Matrix3x3<T>	m_rotation;
		Vector3<T>		m_scale;
	};
	/*----------------------------------------------------------------*/

}
