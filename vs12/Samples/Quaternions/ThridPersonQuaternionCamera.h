#pragma once
#include "BowRenderer.h"
#include "BowCore.h"

class ThirdPersonQuaternionCamera : public Bow::Renderer::Camera
{
public:
	ThirdPersonQuaternionCamera(const Bow::Core::Vector3<double>& cameraPosition, const Bow::Core::Vector3<double>& lookAtPoint, const Bow::Core::Vector3<double>& worldUp, unsigned int width, unsigned int height);
	~ThirdPersonQuaternionCamera();

	Bow::Core::Vector3<float> GetPosition() {
		return m_Position;
	};

	void setTrackballPhiDelta(const float phiDelta) { m_TrackballPhi = phiDelta; }
	void setTrackballThetaDelta(const float thetaDelta) { m_TrackballTheta = thetaDelta; }
	void setTargetYawDelta(const float targetYaw) { m_TargetYaw = targetYaw; }
	void setTargetPitchDelta(const float targetPich) { m_TargetPitch = targetPich; }
	void setTargetRollDelta(const float targetRoll) { m_TargetRoll = targetRoll; }

	void setTargetPosition(const Bow::Core::Vector3<float> targetPosition) { m_TargetPosition = targetPosition; }

	void setTrackballCameraModeActive();
	void setTrackballCameraModeInactive();

	void updateCameraPoistion();
	void updateCameraRotation();

private:
	void updateCameraThirdPersonRotation(const float yaw, const float pitch, const float roll);
	void updateCameraTrackballRotation(const float yaw, const float pitch, const float roll);
	void updateInterpolatedRotationMatrix(const float yaw, const float pitch, const float roll, const Bow::Core::Quaternion<float> targetQuaternion);

	// For Quaternion conversion
	Bow::Core::Matrix3D<float> constructPitchMatrix(const float pitch);	/// X-axis rotation matrix
	Bow::Core::Matrix3D<float> constructYawMatrix(const float yaw);	/// Y-axis rotation matrix
	Bow::Core::Matrix3D<float> constructRollMatrix(const float roll); /// Z-axis rotation matrix

	Bow::Core::Vector3<float> m_Position;
	Bow::Core::Vector3<float> m_TargetPosition;
	Bow::Core::Vector3<float> m_InitialDir;
	Bow::Core::Vector3<float> m_CurrentDir;
	Bow::Core::Vector3<float> m_Up;

	float m_DistanceToTarget;
	float m_TargetYaw;
	float m_TargetPitch;
	float m_TargetRoll;
	float m_InterpolationParameter;

	float m_TrackballPhi;
	float m_TrackballTheta;

	// For camera rotation
	Bow::Core::Matrix3D<float> m_TrackballRotationMatrix;
	Bow::Core::Matrix3D<float> m_ThirdPersonRotationMatrix;
	Bow::Core::Matrix3D<float> m_RotationMatrix;
	Bow::Core::Matrix3D<float> m_InterpolatedRotationMatrix;

	float m_Speed;             // Geschwindigkeit
	float m_ThetaSens;         // Empfindlichkeit Rotation
	float m_PhiSens;
	float m_AccSens;           // Empfindlichkeit Beschleunigung

	Bow::Core::Quaternion<float> m_StartQuaternion;
	Bow::Core::Quaternion<float> m_InterpolatedQuaternion;
	Bow::Core::Quaternion<float> m_TargetQuaternion;

	// For the different camera modes
	bool m_TrackballMode;

	Bow::Core::Quaternion<float> m_TrackballReferenceQuaternion;
};