#pragma once
#include "BowRenderer.h"
#include "BowCore.h"


class FirstPersonCamera : public Bow::Renderer::Camera
{
public:
	FirstPersonCamera(const Bow::Core::Vector3<double>& cameraPosition, const Bow::Core::Vector3<double>& lookAtPoint, const Bow::Core::Vector3<double>& worldUp, unsigned int width, unsigned int height);
	~FirstPersonCamera();

	Bow::Core::Vector3<float> GetPosition() {
		return m_Position; 
	};

	void MoveForward(float deltaTime);
	void MoveBackward(float deltaTime);
	void MoveRight(float deltaTime);
	void MoveLeft(float deltaTime);
	void MoveUp(float deltaTime);
	void MoveDown(float deltaTime);
	void rotate(float deltaX, float deltaY);

private:
	void calcViewDirection();

	Bow::Core::Vector3<float> m_Position;
	Bow::Core::Vector3<float> m_Dir;
	Bow::Core::Vector3<float> m_Up;

	float m_Theta, m_Phi;       // Blickrichtung als (theta,phi) Winkel

	float m_Speed;             // Geschwindigkeit
	float m_ThetaSens;         // Empfindlichkeit Rotation
	float m_PhiSens;
	float m_AccSens;           // Empfindlichkeit Beschleunigung
};