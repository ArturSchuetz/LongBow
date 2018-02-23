#include "ThridPersonQuaternionCamera.h"

using namespace Bow;

// Konstruktor: Default Werte setzen
ThirdPersonQuaternionCamera::ThirdPersonQuaternionCamera(const Bow::Core::Vector3<double>& cameraPosition, const Bow::Core::Vector3<double>& lookAtPoint, const Bow::Core::Vector3<double>& worldUp, unsigned int width, unsigned int height)
	: Renderer::Camera(cameraPosition, lookAtPoint, worldUp, width, height),
	m_Position(cameraPosition),
	m_Up(worldUp)
{
	m_InitialDir.Set(lookAtPoint.x - cameraPosition.x, lookAtPoint.y - cameraPosition.y, lookAtPoint.z - cameraPosition.z);
	m_DistanceToTarget = m_InitialDir.Length();
	m_InitialDir.Normalize();
	m_CurrentDir = m_InitialDir;

	m_InterpolationParameter = 0.0f;

	m_ThetaSens = 0.01f;        // Empfindlichkeit der Steuerung
	m_PhiSens = 0.01f;
	m_Speed = 0.01f;

	m_TrackballPhi = 0.0f;
	m_TrackballTheta = 0.0f;

	m_TrackballRotationMatrix = Bow::Core::Matrix3D<float>();
	m_ThirdPersonRotationMatrix = Bow::Core::Matrix3D<float>();
	m_RotationMatrix = Bow::Core::Matrix3D<float>();
	m_InterpolatedRotationMatrix = Bow::Core::Matrix3D<float>();

	m_StartQuaternion = Bow::Core::Quaternion<float>();
	m_InterpolatedQuaternion = Bow::Core::Quaternion<float>();
	m_TargetQuaternion = Bow::Core::Quaternion<float>();

	m_TrackballMode = false;

	m_TrackballReferenceQuaternion = Bow::Core::Quaternion<float>();
}

ThirdPersonQuaternionCamera::~ThirdPersonQuaternionCamera()
{
}

void ThirdPersonQuaternionCamera::updateCameraPoistion()
{
	m_Position = m_TargetPosition - (m_CurrentDir * m_DistanceToTarget);
}

Bow::Core::Matrix3D<float> ThirdPersonQuaternionCamera::constructPitchMatrix(float pitch)
{
	Bow::Core::Matrix3D<float> rotationAxisXMatrix;

	rotationAxisXMatrix._11 = 1.0;	rotationAxisXMatrix._12 = 0.0;			rotationAxisXMatrix._13 = 0.0;				rotationAxisXMatrix._14 = 0.0;
	rotationAxisXMatrix._21 = 0.0;	rotationAxisXMatrix._22 = cos(pitch);	rotationAxisXMatrix._23 = sin(pitch);		rotationAxisXMatrix._24 = 0.0;
	rotationAxisXMatrix._31 = 0.0;  rotationAxisXMatrix._32 = -sin(pitch);	rotationAxisXMatrix._33 = std::cos(pitch);	rotationAxisXMatrix._34 = 0.0;
	rotationAxisXMatrix._41 = 0.0;	rotationAxisXMatrix._42 = 0.0;			rotationAxisXMatrix._43 = 0.0;				rotationAxisXMatrix._44 = 1.0;

	return rotationAxisXMatrix;
}

Bow::Core::Matrix3D<float> ThirdPersonQuaternionCamera::constructYawMatrix(float yaw)
{
	Bow::Core::Matrix3D<float> rotationAxisYMatrix;

	rotationAxisYMatrix._11 = cos(yaw);		rotationAxisYMatrix._12 = 0.0; rotationAxisYMatrix._13 = sin(yaw);	rotationAxisYMatrix._14 = 0.0;
	rotationAxisYMatrix._21 = 0.0;			rotationAxisYMatrix._22 = 1.0; rotationAxisYMatrix._23 = 0.0;		rotationAxisYMatrix._24 = 0.0;
	rotationAxisYMatrix._31 = -sin(yaw);	rotationAxisYMatrix._32 = 0.0; rotationAxisYMatrix._33 = cos(yaw);	rotationAxisYMatrix._34 = 0.0;
	rotationAxisYMatrix._41 = 0.0;			rotationAxisYMatrix._42 = 0.0; rotationAxisYMatrix._43 = 0.0;		rotationAxisYMatrix._44 = 1.0;

	return rotationAxisYMatrix;
}

Bow::Core::Matrix3D<float> ThirdPersonQuaternionCamera::constructRollMatrix(float roll)
{
	Bow::Core::Matrix3D<float> rotationAxisZMatrix;

	rotationAxisZMatrix._11 = cos(roll);	rotationAxisZMatrix._12 = -sin(roll);	rotationAxisZMatrix._13 = 0.0;	rotationAxisZMatrix._14 = 0.0;
	rotationAxisZMatrix._21 = sin(roll);	rotationAxisZMatrix._22 = cos(roll);	rotationAxisZMatrix._23 = 0.0;	rotationAxisZMatrix._24 = 0.0;
	rotationAxisZMatrix._31 = 0.0;			rotationAxisZMatrix._32 = 0.0;			rotationAxisZMatrix._33 = 1.0;	rotationAxisZMatrix._34 = 0.0;
	rotationAxisZMatrix._41 = 0.0;			rotationAxisZMatrix._42 = 0.0;			rotationAxisZMatrix._43 = 0.0;	rotationAxisZMatrix._44 = 1.0;

	return rotationAxisZMatrix;
}

void ThirdPersonQuaternionCamera::setTrackballCameraModeActive()
{
	if (!m_TrackballMode)
	{
		m_TrackballMode = true;
		m_TrackballRotationMatrix = Bow::Core::Matrix3D<float>();
		m_TrackballReferenceQuaternion = Bow::Core::Quaternion<float>(m_RotationMatrix);
	}
}

void ThirdPersonQuaternionCamera::setTrackballCameraModeInactive()
{
	if (m_TrackballMode)
	{
		m_TrackballMode = false;
	}
}

void ThirdPersonQuaternionCamera::updateCameraThirdPersonRotation(const float yaw, const float pitch, const float roll)
{
	m_ThirdPersonRotationMatrix *= constructRollMatrix(m_TargetRoll) * constructPitchMatrix(m_TargetPitch) * constructYawMatrix(m_TargetYaw);

	Bow::Core::Quaternion<float> targetQuaternion = Bow::Core::Quaternion<float>(m_ThirdPersonRotationMatrix);

	targetQuaternion = targetQuaternion.Normalize(targetQuaternion);

	if (!m_TrackballMode)
	{
		updateInterpolatedRotationMatrix(yaw, pitch, roll, targetQuaternion);
	}

	m_RotationMatrix = m_ThirdPersonRotationMatrix;
}

void ThirdPersonQuaternionCamera::updateCameraTrackballRotation(const float yaw, const float pitch, const float roll)
{
	if (m_TrackballMode)
	{
		m_TrackballRotationMatrix *= constructRollMatrix(roll) * constructPitchMatrix(pitch) * constructYawMatrix(yaw);

		Bow::Core::Quaternion<float> targetQuaternion = Bow::Core::Quaternion<float>(m_TrackballRotationMatrix);

		targetQuaternion = targetQuaternion * m_TrackballReferenceQuaternion;

		targetQuaternion = targetQuaternion.Normalize(targetQuaternion);

		updateInterpolatedRotationMatrix(yaw, pitch, roll, targetQuaternion);

		m_RotationMatrix = m_TrackballRotationMatrix;
	}
}

void ThirdPersonQuaternionCamera::updateInterpolatedRotationMatrix(const float yaw, const float pitch, const float roll, const Bow::Core::Quaternion<float> targetQuaternion)
{
	m_TargetQuaternion = targetQuaternion;

	if (m_StartQuaternion.Dot(m_StartQuaternion, m_TargetQuaternion) < 0.0)
	{
		m_TargetQuaternion = -1.0f * m_TargetQuaternion;
	}

	float t = 0.01f;

	m_InterpolatedQuaternion = Bow::Core::Quaternion<float>(m_StartQuaternion.Slerp(t, m_StartQuaternion, m_TargetQuaternion));

	m_InterpolatedRotationMatrix = m_InterpolatedQuaternion.To3DMatrix();
}

void ThirdPersonQuaternionCamera::updateCameraRotation()
{
	updateCameraThirdPersonRotation(m_TargetYaw, m_TargetPitch, m_TargetRoll);
	updateCameraTrackballRotation(m_TrackballPhi, m_TrackballTheta, 0.0f);

	Bow::Core::Vector4<float> newDir = m_InterpolatedRotationMatrix * Bow::Core::Vector4<float>(m_InitialDir.x, m_InitialDir.y, m_InitialDir.z, 0.0f);
	newDir.Normalize();
	m_CurrentDir = Bow::Core::Vector3<float>(newDir.x, newDir.y, newDir.z);
	m_CurrentDir.Normalize();

	// TODO: Replace this by storing last up vector computing a current right vector and then computing a current up vector with cross products (because less operations).
	Bow::Core::Vector4<float> newUp = m_InterpolatedRotationMatrix * Bow::Core::Vector4<float>(m_Up.x, m_Up.y, m_Up.z, 0.0f);
	newUp.Normalize();

	SetViewLookAt(m_Position, m_Position + m_CurrentDir, Bow::Core::Vector3<float>(newUp.x, newUp.y, newUp.z));

	m_StartQuaternion = m_InterpolatedQuaternion;
}