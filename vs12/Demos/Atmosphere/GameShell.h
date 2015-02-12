#pragma once
#include "BowRenderer.h"
#include "BowInput.h"
#include "BowCore.h"

using namespace Bow;

class GameShell
{
public:
	GameShell();
	~GameShell();

	bool Init(void);
	void Release(void);

	void PrepareShader(void);

	void Update(DWORD deltaTime);
	void Render(void);

	bool IsRunning(void) const
	{
		return !m_window->VShouldClose() && !m_keyboard->VIsPressed(Input::Key::K_ESCAPE);
	}

private:
	Core::MeshPtr m_sceneMesh;
	Renderer::VertexArrayPtr m_sceneVA;

	Renderer::RenderDevicePtr m_device;
	Renderer::GraphicsWindowPtr m_window;
	Renderer::RenderContextPtr m_contextOGL;
	Renderer::ShaderProgramPtr m_SkyShaderProgram;
	Renderer::ShaderProgramPtr m_GroundShaderProgram;
	Renderer::ClearState m_clearState;

	Renderer::Texture2DPtr m_diffuse_texture;
	Renderer::Texture2DPtr m_night_texture;

	Renderer::TextureSamplerPtr m_textureSampler;
	Renderer::RenderState m_renderStateGlobe;
	Renderer::RenderState m_renderStateAtmosphere;

	Input::KeyboardPtr m_keyboard;
	Input::MousePtr m_mouse;

	Core::Matrix3D<float> globeWorldMat;
	Core::Matrix3D<float> skyWorldMat;
	Core::Vector3<float> CameraPosition;

	Renderer::Camera *m_camera;
	Core::Vector2<long> m_lastCursorPosition;
};