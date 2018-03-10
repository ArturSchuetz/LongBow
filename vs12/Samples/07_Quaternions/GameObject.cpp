#pragma once
#include "GameObject.h"
#include "BowQuaternion.h"

GameObject::GameObject(const Vector3<float> position, const Vector3<float> lookAt, const Vector3<float> up, const Vector3<float> scaling)
{
	m_position = position;

	m_inititalDir = lookAt - position;
	m_inititalDir.Normalize();
	m_currentDir = m_inititalDir;
	m_initialUp = up;
	m_initialUp.Normalize();
	m_currentUp = m_initialUp;
	m_currentRight = CrossP(m_currentUp, m_currentDir);
	m_currentRight.Normalize();

	m_scalingMatrix = Matrix3D<float>();
	m_scalingMatrix._11 = scaling.x;
	m_scalingMatrix._22 = scaling.y;
	m_scalingMatrix._33 = scaling.z;
	m_initialRotationMatrix = Matrix3D<float>();
	m_currentRotationMatrix = Matrix3D<float>();
	m_translationMatrix = Matrix3D<float>();
}

void GameObject::setMesh(const std::string meshPath, const Vector3<float> orientationPhiThetaPsi, const ShaderProgramPtr shaderProgram, const RenderContextPtr contextOGL, const BufferHint bufferHint)
{
	m_mesh = MeshManager::GetInstance().Load(meshPath);

	m_meshAttr = m_mesh->CreateAttribute("in_Position", "in_Normal", "in_TexCoord");

	m_vertexArray = contextOGL->VCreateVertexArray(m_meshAttr, shaderProgram->VGetVertexAttributes(), bufferHint);
}

void GameObject::setScaling(const Vector3<float> scalingVector)
{
	m_scaling = scalingVector;
}

void GameObject::setRotation(float phi, float theta, float psi)
{
	m_initialOrientationPhi = phi;
	m_initialOrientationTheta = theta;
	m_initialOrientationPsi = psi;
}

void GameObject::updateTranslation(const float translationDeltaFactor)
{
	m_position += m_currentDir * translationDeltaFactor;
}

void GameObject::updateThirdPersonCamera(ThirdPersonQuaternionCamera* camera, const float cameraPhiDelta, const float cameraThetaDelta)
{
	// Set target position
	camera->setTargetPosition(m_position);

	// Set target orientation values
	camera->setTargetYawDelta(m_phiDelta);
	camera->setTargetPitchDelta(m_thetaDelta);
	camera->setTargetRollDelta(m_psiDelta);

	// Set trackball values
	camera->setTrackballPhiDelta(cameraPhiDelta);
	camera->setTrackballThetaDelta(cameraThetaDelta);

	// Update camera rotation
	camera->updateCameraRotation();

	camera->updateCameraPoistion();
}

bow::Matrix3D<float> GameObject::constructPitchMatrix(const float pitch)
{
	bow::Matrix3D<float> rotationAxisXMatrix;

	rotationAxisXMatrix._11 = 1.0;	rotationAxisXMatrix._12 = 0.0;			rotationAxisXMatrix._13 = 0.0;				rotationAxisXMatrix._14 = 0.0;
	rotationAxisXMatrix._21 = 0.0;	rotationAxisXMatrix._22 = cos(pitch);	rotationAxisXMatrix._23 = sin(pitch);		rotationAxisXMatrix._24 = 0.0;
	rotationAxisXMatrix._31 = 0.0;  rotationAxisXMatrix._32 = -sin(pitch);	rotationAxisXMatrix._33 = std::cos(pitch);	rotationAxisXMatrix._34 = 0.0;
	rotationAxisXMatrix._41 = 0.0;	rotationAxisXMatrix._42 = 0.0;			rotationAxisXMatrix._43 = 0.0;				rotationAxisXMatrix._44 = 1.0;

	return rotationAxisXMatrix;
}

bow::Matrix3D<float> GameObject::constructYawMatrix(const float yaw)
{
	bow::Matrix3D<float> rotationAxisYMatrix;

	rotationAxisYMatrix._11 = cos(yaw);		rotationAxisYMatrix._12 = 0.0; rotationAxisYMatrix._13 = sin(yaw);	rotationAxisYMatrix._14 = 0.0;
	rotationAxisYMatrix._21 = 0.0;			rotationAxisYMatrix._22 = 1.0; rotationAxisYMatrix._23 = 0.0;		rotationAxisYMatrix._24 = 0.0;
	rotationAxisYMatrix._31 = -sin(yaw);	rotationAxisYMatrix._32 = 0.0; rotationAxisYMatrix._33 = cos(yaw);	rotationAxisYMatrix._34 = 0.0;
	rotationAxisYMatrix._41 = 0.0;			rotationAxisYMatrix._42 = 0.0; rotationAxisYMatrix._43 = 0.0;		rotationAxisYMatrix._44 = 1.0;

	return rotationAxisYMatrix;
}

bow::Matrix3D<float> GameObject::constructRollMatrix(const float roll)
{
	bow::Matrix3D<float> rotationAxisZMatrix;

	rotationAxisZMatrix._11 = cos(roll);	rotationAxisZMatrix._12 = -sin(roll);	rotationAxisZMatrix._13 = 0.0;	rotationAxisZMatrix._14 = 0.0;
	rotationAxisZMatrix._21 = sin(roll);	rotationAxisZMatrix._22 = cos(roll);	rotationAxisZMatrix._23 = 0.0;	rotationAxisZMatrix._24 = 0.0;
	rotationAxisZMatrix._31 = 0.0;			rotationAxisZMatrix._32 = 0.0;			rotationAxisZMatrix._33 = 1.0;	rotationAxisZMatrix._34 = 0.0;
	rotationAxisZMatrix._41 = 0.0;			rotationAxisZMatrix._42 = 0.0;			rotationAxisZMatrix._43 = 0.0;	rotationAxisZMatrix._44 = 1.0;

	return rotationAxisZMatrix;
}

void GameObject::updateRotation(float yaw, float pitch, float roll)
{
	m_phiDelta = yaw;
	m_thetaDelta = pitch;
	m_psiDelta = roll;

	// Complete yaw, pitch, roll rotation matrix 
	m_currentRotationMatrix *= constructRollMatrix(roll) * constructPitchMatrix(pitch) * constructYawMatrix(yaw);

	bow::Vector4<float> newDir = m_currentRotationMatrix * m_initialRotationMatrix * bow::Vector4<float>(m_inititalDir.x, m_inititalDir.y, m_inititalDir.z, 0.0f);

	m_currentDir = Vector3<float>(newDir.x, newDir.y, newDir.z);
	m_currentDir.Normalize();
}