#pragma once
#include "BowRenderer.h"
#include "BowInput.h"
#include "BowCore.h"
#include "BowResources.h"

#include "FirstPersonCamera.h"
#include "WaveletSphere.h"

using namespace Bow;

class Application
{
public:
	Application();
	~Application();

	bool Init(void);
	void PrepareScene(void);
	void Release(void);

	void Update(float deltaTime);
	void Render(void);

	bool IsRunning(void) const
	{
		return !m_window->VShouldClose() && !m_keyboard->VIsPressed(Input::Key::K_ESCAPE);
	}

private:
	Renderer::RenderDevicePtr	m_device;
	Renderer::GraphicsWindowPtr	m_window;
	Renderer::RenderContextPtr	m_contextOGL;

	Renderer::ShaderProgramPtr	m_shaderProgram;
	Renderer::Texture2DPtr		m_terrainDiffuseTexture;
	Renderer::TextureSamplerPtr m_sampler;

	Renderer::ClearState	m_clearState;

	Input::KeyboardPtr	m_keyboard;
	Input::MousePtr		m_mouse;

	bool				m_pressedQ;
	bool				m_pressedF1;
	bool				m_pressedF2;
	bool				m_pressedF3;
	bool				m_pressedF4;
	bool				m_pressedF5;
	bool				m_pressedReturn;

	bool				m_updateFrustum;
	float				m_moveSpeed;
	FirstPersonCamera	*m_camera;
	Vector2<long> m_lastCursorPosition;

	bool				m_renderFilled;
	WaveletSphere		m_Sphere;
};