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
	RenderDevicePtr deviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(RenderDeviceAPI::OpenGL3x);
	if (deviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 600, "Mesh Rendering Sample", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}
	RenderContextPtr contextOGL = windowOGL->VGetContext();
	ShaderProgramPtr shaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// Vertex Array from Mesh

	MeshPtr mesh = MeshManager::GetInstance().Load("../Data/models/sponza/sponza.obj");
	std::vector<SubMesh*> subMeshes = mesh->GetSubMeshes();

	MeshAttribute meshAttr = mesh->CreateAttribute("in_Position", "in_Normal", "in_TexCoord");
	VertexArrayPtr vertexArray = contextOGL->VCreateVertexArray(meshAttr, shaderProgram->VGetVertexAttributes(), BufferHint::StaticDraw);

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Camera

	Vector3<float> position = Vector3<float>(400.0f, 200.0f, 0.0f);
	Vector3<float> lookAt = Vector3<float>(0.0f, 0.0f, 0.0f);
	Vector3<float> upVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	FirstPersonCamera camera = FirstPersonCamera(position, lookAt, upVector, windowOGL->VGetWidth(), windowOGL->VGetHeight());
	camera.SetClippingPlanes(0.1, 10000.0);

	///////////////////////////////////////////////////////////////////
	// Input

	KeyboardPtr keyboard = InputDeviceManager::GetInstance().CreateKeyboardObject(windowOGL);
	if (keyboard == nullptr)
	{
		return false;
	}

	MousePtr mouse = InputDeviceManager::GetInstance().CreateMouseObject(windowOGL);
	if (mouse == nullptr)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.RasterizationMode = RasterizationMode::Fill;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 0.0f));

	BasicTimer timer;
	float m_moveSpeed;
	Vector2<long> lastCursorPosition;

	while (!windowOGL->VShouldClose())
	{
		// =======================================================
		// Render Frame

		contextOGL->VClear(clearState);

		contextOGL->VSetViewport(Viewport(0, 0, windowOGL->VGetWidth(), windowOGL->VGetHeight()));
		camera.SetResolution(windowOGL->VGetWidth(), windowOGL->VGetHeight());
		shaderProgram->VSetUniform("u_ModelView", (Matrix4x4<float>)camera.CalculateWorldView(worldMat));
		shaderProgram->VSetUniform("u_Proj", (Matrix4x4<float>)camera.CalculateProjection());

		for (unsigned int i = 0; i < subMeshes.size(); i++)
		{
			contextOGL->VDraw(PrimitiveType::Triangles, subMeshes[i]->GetStartIndex(), subMeshes[i]->GetNumIndices(), vertexArray, shaderProgram, renderState);
		}

		contextOGL->VSwapBuffers();

		// =======================================================

		timer.Update();

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
			windowOGL->VHideCursor();
			Vector3<long> moveVec = mouse->VGetRelativePosition();
			camera.rotate((float)moveVec.x, (float)moveVec.y);
			mouse->VSetCursorPosition(lastCursorPosition.x, lastCursorPosition.y);
		}
		else
		{
			windowOGL->VShowCursor();
		}

		lastCursorPosition = mouse->VGetAbsolutePosition();
	}
	return 0;
}