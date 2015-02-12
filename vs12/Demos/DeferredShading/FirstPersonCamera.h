#pragma once
#include "BowRenderer.h"
#include "BowCore.h"

#include <Windows.h>

class FirstPersonCamera : public Bow::Renderer::Camera
{
public:
	FirstPersonCamera(const Bow::Core::Vector3<double>& cameraPosition, const Bow::Core::Vector3<double>& lookAtPoint, const Bow::Core::Vector3<double>& worldUp, unsigned int width, unsigned int height);
	~FirstPersonCamera();

	void MoveForward(DWORD deltaTime);
	void MoveBackward(DWORD deltaTime);
	void MoveRight(DWORD deltaTime);
	void MoveLeft(DWORD deltaTime);
	void MoveUp(DWORD deltaTime);
	void MoveDown(DWORD deltaTime);
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