#pragma once
#include "BowRenderer.h"
#include "BowInput.h"
#include "BowCore.h"

#include "FirstPersonCamera.h"

using namespace Bow;

class GameShell
{
public:
	GameShell();
	~GameShell();

	bool Init(void);
	void PrepareScreenFillingQuad(void);
	void PrepareLightVolumes(void);
	void PrepareScene(void);
	void Release(void);

	void Update(DWORD deltaTime);
	void Render(void);

	bool IsRunning(void) const
	{
		return !m_window->VShouldClose() && !m_keyboard->VIsPressed(Input::Key::K_ESCAPE);
	}

private:
	Core::MeshPtr m_sceneMesh;
	Core::MeshPtr m_sphereMesh;
	Renderer::VertexArrayPtr m_sceneVA;
	Renderer::VertexArrayPtr m_QuadVA;
	Renderer::VertexArrayPtr m_LightVA;

	Renderer::RenderDevicePtr m_device;
	Renderer::GraphicsWindowPtr m_window;
	Renderer::RenderContextPtr m_contextOGL;

	Renderer::ShaderProgramPtr m_ambientShaderProgram;
	Renderer::ShaderProgramPtr m_fillGBufferShaderProgram;
	Renderer::ShaderProgramPtr m_compositeShaderProgram;

	Renderer::ClearState m_clearState;
	Renderer::RenderState m_renderStateSolid;
	Renderer::RenderState m_renderStateLignt;

	Renderer::TextureSamplerPtr m_textureSampler;

	int GBuffer_Width, GBuffer_Height;

	Renderer::FramebufferPtr m_GeometryBuffer;

	Renderer::Texture2DPtr m_AlbedoBuffer;
	Renderer::Texture2DPtr m_DepthBuffer;
	Renderer::Texture2DPtr m_NormalBuffer;

	int AlbedoRT_Location, NormalRT_Location;

	Input::KeyboardPtr m_keyboard;
	Input::MousePtr m_mouse;

	FirstPersonCamera *m_camera;
	Core::Vector2<long> m_lastCursorPosition;
	std::vector<Core::Vector3<float>> m_lightPositions;
};