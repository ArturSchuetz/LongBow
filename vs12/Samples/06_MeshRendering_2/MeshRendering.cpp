#include "BowRenderer.h"
#include "BowResources.h"
#include "BowInput.h"

#include "FirstPersonCamera.h"

#include <cstdint>
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "resource.h"

using namespace bow;



std::string LoadShaderFromResouce(int name)
{
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
	HGLOBAL rcData = LoadResource(handle, rc);
	return std::string(static_cast<const char*>(LockResource(rcData)));
}

int main()
{
	// Creating Render Device
	RenderDevicePtr DeviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(RenderDeviceAPI::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "Mesh Rendering Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}
	RenderContextPtr ContextOGL = WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// Vertex Array from Mesh

	MeshPtr mesh = MeshManager::GetInstance().Load("../Data/sponza.obj");

	MeshAttribute meshAttr = mesh->CreateAttribute("in_Position", "in_Normal", "in_TexCoord");

	VertexArrayPtr VertexArray = ContextOGL->VCreateVertexArray(meshAttr, ShaderProgram->VGetVertexAttributes(), BufferHint::StaticDraw);

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Camera

	Vector3<float> Position = Vector3<float>(0.0f, 200.0f, 0.0f);
	Vector3<float> LookAt = Vector3<float>(0.0f, 0.0f, 0.0f);
	Vector3<float> UpVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	FirstPersonCamera camera = FirstPersonCamera(Position, LookAt, UpVector, WindowOGL->VGetWidth(), WindowOGL->VGetHeight());
	camera.SetClippingPlanes(0.1, 10000.0);

	///////////////////////////////////////////////////////////////////
	// Input

	KeyboardPtr keyboard = InputDeviceManager::GetInstance().CreateKeyboardObject(WindowOGL);
	if (keyboard == nullptr)
	{
		return false;
	}

	MousePtr mouse = InputDeviceManager::GetInstance().CreateMouseObject(WindowOGL);
	if (mouse == nullptr)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.RasterizationMode = RasterizationMode::Line;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 2000.0f));

	BasicTimer timer;
	float m_moveSpeed;
	Vector2<long> lastCursorPosition;

	while (!WindowOGL->VShouldClose())
	{
		// =======================================================
		// Render Frame

		ContextOGL->VClear(clearState);

		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));
		camera.SetResolution(WindowOGL->VGetWidth(), WindowOGL->VGetHeight());
		ShaderProgram->VSetUniform("u_ModelViewProj", (Matrix4x4<float>)camera.CalculateWorldViewProjection(worldMat));

		ContextOGL->VDraw(PrimitiveType::Triangles, 0, meshAttr.Indices->Size(), VertexArray, ShaderProgram, renderState);

		ContextOGL->VSwapBuffers();
		WindowOGL->VPollWindowEvents();
		// =======================================================

		timer.Update();

		WindowOGL->VPollWindowEvents();
		keyboard->VUpdate();
		mouse->VUpdate();

		m_moveSpeed = 10000.0;

		if (keyboard->VIsPressed(Key::K_W))
		{
			if (keyboard->VIsPressed(Key::K_LSHIFT))
			{
				camera.MoveForward(m_moveSpeed * (float)timer.GetDelta() * 2);
			}
			else
			{
				camera.MoveForward(m_moveSpeed * (float)timer.GetDelta());
			}
		}

		if (keyboard->VIsPressed(Key::K_S))
		{
			camera.MoveBackward(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_D))
		{
			camera.MoveRight(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_A))
		{
			camera.MoveLeft(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_SPACE))
		{
			camera.MoveUp(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_LCONTROL))
		{
			camera.MoveDown(m_moveSpeed * (float)timer.GetDelta());
		}

		if (mouse->VIsPressed(MouseButton::MOFS_BUTTON1))
		{
			WindowOGL->VHideCursor();
			Vector3<long> moveVec = mouse->VGetRelativePosition();
			camera.rotate((float)moveVec.x, (float)moveVec.y);
			mouse->VSetCursorPosition(lastCursorPosition.x, lastCursorPosition.y);
		}
		else
		{
			WindowOGL->VShowCursor();
		}

		lastCursorPosition = mouse->VGetAbsolutePosition();
	}
	return 0;
}