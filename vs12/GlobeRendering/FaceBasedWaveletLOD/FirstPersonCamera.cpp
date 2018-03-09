#include "FirstPersonCamera.h"
#include "BowMath.h"

using namespace Bow;

const float thetaEps = 0.01f;
const float Pi = 3.141592f;

// Konstruktor: Default Werte setzen
FirstPersonCamera::FirstPersonCamera(const bow::Vector3<double>& cameraPosition, const bow::Vector3<double>& lookAtPoint, const bow::Vector3<double>& worldUp, unsigned int width, unsigned int height)
	: Renderer::Camera(cameraPosition, lookAtPoint, worldUp, width, height),
	m_Position(cameraPosition),
	m_Up(worldUp)
{
	m_Theta = Pi * 0.5f;           // Kamera waagrecht
	m_Phi = Pi * 0.5f;             // schraeg durchs Gelaende schauen 
	calcViewDirection();       // mDir aus (theta,phi) initialisieren

	m_ThetaSens = 0.01f;        // Empfindlichkeit der Steuerung
	m_PhiSens = 0.01f;
	m_Speed = 0.01f;
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::MoveForward(float deltaTime)
{
	Vector3<float> delta = m_Dir * m_Speed * (float)deltaTime;
	m_Position += delta;

	SetViewLookAt(m_Position, m_Position + m_Dir, m_Up);
}

void FirstPersonCamera::MoveBackward(float deltaTime)
{
	Vector3<float> delta = m_Dir * m_Speed * (float)deltaTime;
	m_Position -= delta;

	SetViewLookAt(m_Position, m_Position + m_Dir, m_Up);
}

void FirstPersonCamera::MoveRight(float deltaTime)
{
	Vector3<float> vcTemp, vcUp;

	// calculate up vector
	float fDot = DotP(m_Up, m_Dir);

	vcTemp = m_Dir * fDot;
	vcUp = m_Up - vcTemp;
	float fL = vcUp.Length();

	// if length too small take normal y axis as up vector
	if (fL < 1e-6f)
	{
		Vector3<float> vcY;
		vcY.Set(0.0f, 1.0f, 0.0f);

		vcTemp = m_Dir * m_Dir.y;
		vcUp = vcY - vcTemp;

		fL = vcUp.Length();

		// if still too small take z axis as up vector
		if (fL < 1e-6f)
		{
			vcY.Set(0.0f, 0.0f, 1.0f);

			vcTemp = m_Dir * m_Dir.z;
			vcUp = vcY - vcTemp;

			// if still too small we are lost         
			fL = vcUp.Length();
			if (fL < 1e-6f)
				return;
		}
	}

	vcUp /= fL;

	// build right vector using cross product
	Vector3<float> vcRight(CrossP(m_Dir, vcUp));

	Vector3<float> delta = vcRight * m_Speed * (float)deltaTime;
	m_Position += delta;

	SetViewLookAt(m_Position, m_Position + m_Dir, m_Up);
}

void FirstPersonCamera::MoveLeft(float deltaTime)
{
	Vector3<float> vcTemp, vcUp;

	// calculate up vector
	float fDot = DotP(m_Up, m_Dir);

	vcTemp = m_Dir * fDot;
	vcUp = m_Up - vcTemp;
	float fL = vcUp.Length();

	// if length too small take normal y axis as up vector
	if (fL < 1e-6f)
	{
		Vector3<float> vcY;
		vcY.Set(0.0f, 1.0f, 0.0f);

		vcTemp = m_Dir * m_Dir.y;
		vcUp = vcY - vcTemp;

		fL = vcUp.Length();

		// if still too small take z axis as up vector
		if (fL < 1e-6f)
		{
			vcY.Set(0.0f, 0.0f, 1.0f);

			vcTemp = m_Dir * m_Dir.z;
			vcUp = vcY - vcTemp;

			// if still too small we are lost         
			fL = vcUp.Length();
			if (fL < 1e-6f)
				return;
		}
	}

	vcUp /= fL;

	// build right vector using cross product
	Vector3<float> vcRight(CrossP(m_Dir, vcUp));

	Vector3<float> delta = vcRight * m_Speed * (float)deltaTime;
	m_Position -= delta;

	SetViewLookAt(m_Position, m_Position + m_Dir, m_Up);
}

void FirstPersonCamera::MoveUp(float deltaTime)
{
	Vector3<float> delta = m_Up * m_Speed * (float)deltaTime;
	m_Position += delta;

	SetViewLookAt(m_Position, m_Position + m_Dir, m_Up);
}

void FirstPersonCamera::MoveDown(float deltaTime)
{
	Vector3<float> delta = m_Up * m_Speed * (float)deltaTime;
	m_Position -= delta;

	SetViewLookAt(m_Position, m_Position + m_Dir, m_Up);
}

// Hilfsfunktion: Richtungsvektor aus (theta, phi) berechnen
void FirstPersonCamera::calcViewDirection()
{
	m_Dir.Set(sin(m_Theta) * cos(m_Phi), cos(m_Theta), sin(m_Theta) * sin(m_Phi));

	SetViewLookAt(m_Position, m_Position + m_Dir, m_Up);
}

// Kamera drehen
void FirstPersonCamera::rotate(float deltaX, float deltaY)
{
	float deltaTheta = m_ThetaSens * deltaY;        // Schrittwinkel fuer Theta
	m_Theta += deltaTheta;

	// Gueltigkeitsbereich testen
	if (m_Theta < thetaEps)
		m_Theta = thetaEps;
	else if (m_Theta > Pi - thetaEps)
		m_Theta = Pi - thetaEps; // nie entlang up-Vector fliegen

	float deltaPhi = m_PhiSens * deltaX;        // Schrittwinkel fuer Phi
	m_Phi += deltaPhi;

	// Gueltigkeitsbereich testen
	if (m_Phi < 0)
		m_Phi += 2 * Pi;
	else if (m_Phi > 2 * Pi)
		m_Phi -= 2 * Pi;

	calcViewDirection();        // (theta,phi) in (x,y,z) umrechnen
}
