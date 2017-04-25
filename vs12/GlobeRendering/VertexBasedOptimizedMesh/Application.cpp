#include "Application.h"
#include "resource.h"

#include <iostream>

// --------------------------------------
// GLOBALS
// --------------------------------------

std::string LoadShaderFromResouce(int name)
{
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
	HGLOBAL rcData = LoadResource(handle, rc);
	return std::string(static_cast<const char*>(LockResource(rcData)));
}

Application::Application()
{
	m_camera = nullptr;
	m_renderFilled = false;
}

Application::~Application()
{
	Release();
}

bool Application::Init(void)
{
	std::cout << "Instuctions:" << std::endl << std::endl;
	std::cout << "\tUse W,A,S,D to move and press shift to move faster." << std::endl;
	std::cout << "\tUse spacebar to move up and left control to move down." << std::endl;
	std::cout << "\tMove mouse while pressing the right mousebutton to look around." << std::endl;
	std::cout << std::endl << std::endl;

	// Creating Render Device
	m_device = Renderer::RenderDeviceManager::GetInstance().GetOrCreateDevice(Renderer::API::OpenGL3x);
	if (m_device == nullptr)
	{
		return false;
	}

	// Creating Window
	m_window = m_device->VCreateWindow(800, 600, "Vertex Based Optimized Mesh", Renderer::WindowType::Windowed);
	if (m_window == nullptr)
	{
		return false;
	}

	m_contextOGL = m_window->VGetContext();

	///////////////////////////////////////////////////////////////////
	// Input
	m_keyboard = Input::InputDeviceManager::GetInstance().CreateKeyboardObject(m_window);
	if (m_keyboard == nullptr)
	{
		return false;
	}

	m_mouse = Input::InputDeviceManager::GetInstance().CreateMouseObject(m_window);
	if (m_mouse == nullptr)
	{
		return false;
	}


	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	m_clearState.Color = Core::ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// RenderState

	PrepareScene();

	return true;
}

void Application::PrepareScene(void)
{
	///////////////////////////////////////////////////////////////////
	// Camera

	Core::Vector3<float> Position = Core::Vector3<float>(0.0f, 0.0f, -3.0f);
	Core::Vector3<float> LookAt = Core::Vector3<float>(0.0f, 0.0f, 0.0f);
	Core::Vector3<float> UpVector = Core::Vector3<float>(0.0f, 1.0f, 0.0f);

	m_camera = new FirstPersonCamera(Position, LookAt, UpVector, m_window->VGetWidth(), m_window->VGetHeight());
	m_camera->SetClippingPlanes(0.00001, 10.0);

	///////////////////////////////////////////////////////////////////
	// Textures

	Renderer::Texture2DPtr terrainDiffuseTexture = m_device->VCreateTexture2DFromFile("../Data/Textures/NASA/world_topo_bathy_200411_3x5400x2700.jpg");
	Renderer::TextureSamplerPtr sampler = m_device->VCreateTexture2DSampler(Renderer::TextureMinificationFilter::Linear, Renderer::TextureMagnificationFilter::Linear, Renderer::TextureWrap::Clamp, Renderer::TextureWrap::Clamp);
	Renderer::ShaderProgramPtr m_shaderProgram = m_device->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	m_moveSpeed = 50.0;

	m_Sphere.Init(m_shaderProgram, "../Data/Textures/NASA/gebco_08_rev_elev_21600x10800.png");
}

void Application::Release(void)
{
	if (m_camera)
	{
		delete m_camera;
	}
}

void Application::Update(float deltaTime)
{
	m_window->VPollWindowEvents();
	m_keyboard->VUpdate();
	m_mouse->VUpdate();

	if (m_keyboard->VIsPressed(Input::Key::K_Q))
	{
		if (!m_pressedQ)
			m_renderFilled = !m_renderFilled;

		m_pressedQ = true;
	}
	else { m_pressedQ = false; }

	m_moveSpeed = (float)((m_camera->GetPosition().Length() - 1.0) * 50.0);

	if (m_keyboard->VIsPressed(Input::Key::K_W))
	{
		if (m_keyboard->VIsPressed(Input::Key::K_LSHIFT))
		{
			m_camera->MoveForward(m_moveSpeed * deltaTime * 2);
		}
		else
		{
			m_camera->MoveForward(m_moveSpeed * deltaTime);
		}
	}

	if (m_keyboard->VIsPressed(Input::Key::K_S))
	{
		m_camera->MoveBackward(m_moveSpeed * deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_D))
	{
		m_camera->MoveRight(m_moveSpeed * deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_A))
	{
		m_camera->MoveLeft(m_moveSpeed * deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_SPACE))
	{
		m_camera->MoveUp(m_moveSpeed * deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_LCONTROL))
	{
		m_camera->MoveDown(m_moveSpeed * deltaTime);
	}

	if (m_mouse->VIsPressed(Input::MouseButton::MOFS_BUTTON1))
	{
		m_window->VHideCursor();
		Core::Vector3<long> moveVec = m_mouse->VGetRelativePosition();
		m_camera->rotate((float)moveVec.x, (float)moveVec.y);
		m_mouse->VSetCursorPosition(m_lastCursorPosition.x, m_lastCursorPosition.y);
	}
	else
	{
		m_window->VShowCursor();
	}

	m_lastCursorPosition = m_mouse->VGetAbsolutePosition();
	m_Sphere.Update(deltaTime);
}

void Application::Render(void)
{
	// =======================================================
	// Prepare camera

	m_camera->SetResolution(m_window->VGetWidth(), m_window->VGetHeight());

	// =======================================================
	// Render Sphere

	m_contextOGL->VClear(m_clearState);

	m_contextOGL->VSetViewport(Renderer::Viewport(0, 0, m_window->VGetWidth(), m_window->VGetHeight()));

	m_Sphere.Render(m_contextOGL, m_camera, m_renderFilled);

	m_contextOGL->VSwapBuffers();

	// =======================================================
}
