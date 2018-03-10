#pragma once
#include "BowRenderer.h"
#include "BowResources.h"
#include "resource.h"
#include "ThridPersonQuaternionCamera.h"

using namespace bow;

class GameObject
{
public:
	GameObject(const Vector3<float> position, const Vector3<float> lookAt, const Vector3<float> up, const Vector3<float> scaling);
	~GameObject() {}

	void setMesh(const std::string meshPath, const Vector3<float> orientationPhiThetaPsi, const ShaderProgramPtr shaderProgram, const RenderContextPtr contextOGL, const BufferHint bufferHint);

	void setScaling(const Vector3<float> scalingVector);

	void setRotation(const float phiDelta, const float thetaDelta, const float psiDelta);
	void updateTranslation(const float translationDeltaFactor);

	void updateThirdPersonCamera(ThirdPersonQuaternionCamera* camera, const float cameraPhiDelta, const float cameraThetaDelta);

	void updateRotation(float yaw, float pitch, float roll);

	Vector3<float> getPosition() { return m_position; }
	MeshAttribute getMeshAttribute() { return m_meshAttr; }
	VertexArrayPtr& getVertexArrayPtr() { return m_vertexArray; }

	//Quaternion test
	Matrix3D<float> getWorldMatrix()
	{
		// Initial rotation matrix construction
		Matrix3D<float> InitialYawAxisRotationMatrix = constructYawMatrix(m_initialOrientationPhi);
		Matrix3D<float> InitialPitchAxisRotationMatrix = constructPitchMatrix(m_initialOrientationTheta);
		Matrix3D<float> InitialRollRotationMatrix = constructRollMatrix(m_initialOrientationPsi);

		m_initialRotationMatrix = InitialRollRotationMatrix * InitialPitchAxisRotationMatrix * InitialYawAxisRotationMatrix;

		// Create translation Matrix
		m_translationMatrix._14 = m_position.x;
		m_translationMatrix._24 = m_position.y;
		m_translationMatrix._34 = m_position.z;

		m_worldMatrix = m_translationMatrix * m_currentRotationMatrix * m_initialRotationMatrix * m_scalingMatrix;

		return m_worldMatrix;
	}

private:
	bow::Matrix3D<float> constructPitchMatrix(const float pitch);		/// X-axis rotation matrix
	bow::Matrix3D<float> constructYawMatrix(const float yaw);			/// Y-axis rotation matrix
	bow::Matrix3D<float> constructRollMatrix(const float roll);		/// Z-axis rotation matrix

																			// Position
	Vector3<float> m_position;

	// Orientation
	Vector3<float> m_inititalDir;
	Vector3<float> m_initialUp;

	Vector3<float> m_currentDir;
	Vector3<float> m_currentUp;
	Vector3<float> m_currentRight;

	float m_initialOrientationPhi;		/// Y-axis rotation 
	float m_initialOrientationTheta;		/// X-axis rotation
	float m_initialOrientationPsi;		///	Z-axis rotation

	float m_phiDelta;		/// Y-axis rotation 
	float m_thetaDelta;		/// X-axis rotation
	float m_psiDelta;		///	Z-axis rotation

	// Scaling
	Vector3<float> m_scaling;

	// Three component matrices of world matrix
	Matrix3D<float> m_scalingMatrix;
	Matrix3D<float> m_translationMatrix;
	Matrix3D<float> m_initialRotationMatrix;
	Matrix3D<float> m_currentRotationMatrix;

	// World matrix
	Matrix3D<float> m_worldMatrix;

	// Model mesh, attribute and vertex array
	MeshPtr m_mesh;
	MeshAttribute m_meshAttr;
	VertexArrayPtr m_vertexArray;

	// Speed sensitivities
	float m_Speed;             // Geschwindigkeit
	float m_PhiSens;         // Empfindlichkeit Rotation
	float m_ThetaSens;
	float m_PsiSens;
	float m_AccSens;           // Empfindlichkeit Beschleunigung
};