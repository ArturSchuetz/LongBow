#pragma once
#include "BowRenderer.h"
#include "BowCore.h"

class ThirdPersonQuaternionCamera : public bow::Camera
{
public:
	ThirdPersonQuaternionCamera(const bow::Vector3<double>& cameraPosition, const bow::Vector3<double>& lookAtPoint, const bow::Vector3<double>& worldUp, unsigned int width, unsigned int height);
	~ThirdPersonQuaternionCamera();

	bow::Vector3<float> GetPosition() {
		return m_Position;
	};

	void setTrackballPhiDelta(const float phiDelta) { m_TrackballPhi = phiDelta; }
	void setTrackballThetaDelta(const float thetaDelta) { m_TrackballTheta = thetaDelta; }
	void setTargetYawDelta(const float targetYaw) { m_TargetYaw = targetYaw; }
	void setTargetPitchDelta(const float targetPich) { m_TargetPitch = targetPich; }
	void setTargetRollDelta(const float targetRoll) { m_TargetRoll = targetRoll; }

	void setTargetPosition(const bow::Vector3<float> targetPosition) { m_TargetPosition = targetPosition; }

	void setTrackballCameraModeActive();
	void setTrackballCameraModeInactive();

	void updateCameraPoistion();
	void updateCameraRotation();

private:
	void updateCameraThirdPersonRotation(const float yaw, const float pitch, const float roll);
	void updateCameraTrackballRotation(const float yaw, const float pitch, const float roll);
	void updateInterpolatedRotationMatrix(const float yaw, const float pitch, const float roll, const bow::Quaternion<float> targetQuaternion);

	// For Quaternion conversion
	bow::Matrix3D<float> constructPitchMatrix(const float pitch);	/// X-axis rotation matrix
	bow::Matrix3D<float> constructYawMatrix(const float yaw);	/// Y-axis rotation matrix
	bow::Matrix3D<float> constructRollMatrix(const float roll); /// Z-axis rotation matrix

	bow::Vector3<float> m_Position;
	bow::Vector3<float> m_TargetPosition;
	bow::Vector3<float> m_InitialDir;
	bow::Vector3<float> m_CurrentDir;
	bow::Vector3<float> m_Up;

	float m_DistanceToTarget;
	float m_TargetYaw;
	float m_TargetPitch;
	float m_TargetRoll;
	float m_InterpolationParameter;

	float m_TrackballPhi;
	float m_TrackballTheta;

	// For camera rotation
	bow::Matrix3D<float> m_TrackballRotationMatrix;
	bow::Matrix3D<float> m_ThirdPersonRotationMatrix;
	bow::Matrix3D<float> m_RotationMatrix;
	bow::Matrix3D<float> m_InterpolatedRotationMatrix;

	float m_Speed;             // Geschwindigkeit
	float m_ThetaSens;         // Empfindlichkeit Rotation
	float m_PhiSens;
	float m_AccSens;           // Empfindlichkeit Beschleunigung

	bow::Quaternion<float> m_StartQuaternion;
	bow::Quaternion<float> m_InterpolatedQuaternion;
	bow::Quaternion<float> m_TargetQuaternion;

	// For the different camera modes
	bool m_TrackballMode;

	bow::Quaternion<float> m_TrackballReferenceQuaternion;
};