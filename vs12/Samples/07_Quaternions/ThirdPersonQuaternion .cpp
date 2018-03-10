#include "BowRenderer.h"
#include "BowResources.h"
#include "BowInput.h"

#include "ThridPersonQuaternionCamera.h"
#include "GameObject.h"

#include <cstdint>
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "resource.h"

//#include "Quaternion.h"

using namespace bow;

//# define M_PI           3.14159265358979323846 

std::string LoadShaderFromResouce(int name)
{
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
	HGLOBAL rcData = LoadResource(handle, rc);
	return std::string(static_cast<const char*>(LockResource(rcData)));
}

// For plane controls
float planeVelocity = 0.0f;
float planeSpeed = 0.04f;
float planeSpeedDamp = 0.04f;
float pitchDelta = 0.0f;
float rollDelta = 0.0f;
float pitchRollDeltaSensitivity = 2.5f;
Vector3<long> moveVec = Vector3<long>();

void planeControls(MousePtr mouse, KeyboardPtr keyboard, BasicTimer timer, GraphicsWindowPtr windowOGL, Vector2<long> lastCursorPosition, ThirdPersonQuaternionCamera* camera)
{
	// Third person keyboard controls 
	if (keyboard->VIsPressed(Key::K_W))
	{
		if (keyboard->VIsPressed(Key::K_LSHIFT))
		{
			planeVelocity += planeSpeed * (float)timer.GetDelta();
		}
		else
		{
			pitchDelta = pitchRollDeltaSensitivity * (float)timer.GetDelta();
		}
	}

	if (keyboard->VIsPressed(Key::K_S))
	{
		if (keyboard->VIsPressed(Key::K_LSHIFT))
		{
			planeVelocity += -planeSpeedDamp * (float)timer.GetDelta();
		}
		else
		{
			pitchDelta = -pitchRollDeltaSensitivity * (float)timer.GetDelta();
		}
	}

	if (keyboard->VIsPressed(Key::K_A))
	{
		rollDelta = pitchRollDeltaSensitivity * (float)timer.GetDelta();
	}

	if (keyboard->VIsPressed(Key::K_D))
	{
		rollDelta = -pitchRollDeltaSensitivity * (float)timer.GetDelta();
	}

	// Trackball mouse controls
	if (mouse->VIsPressed(MouseButton::MOFS_BUTTON1))
	{
		windowOGL->VHideCursor();
		moveVec = mouse->VGetRelativePosition();
		camera->setTrackballCameraModeActive();
		mouse->VSetCursorPosition(lastCursorPosition.x, lastCursorPosition.y);
	}
	else
	{
		windowOGL->VShowCursor();
	}

	lastCursorPosition = mouse->VGetAbsolutePosition();

	if (keyboard->VIsPressed(Key::K_SPACE))
	{
		camera->setTrackballCameraModeInactive();
	}
}

void planeUpdate(GameObject* gameObject, MousePtr mouse, KeyboardPtr keyboard, BasicTimer timer, GraphicsWindowPtr windowOGL, Vector2<long> lastCursorPosition, ThirdPersonQuaternionCamera* camera)
{
	planeControls(mouse, keyboard, timer, windowOGL, lastCursorPosition, camera);
	gameObject->updateTranslation(planeVelocity);
	gameObject->updateRotation(0.0f, pitchDelta, rollDelta);

	pitchDelta = 0.0f;
	rollDelta = 0.0f;
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
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 600, "Triangle Sample", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}
	RenderContextPtr contextOGL = windowOGL->VGetContext();
	ShaderProgramPtr CameraShaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_CAMERAVERTEXSHADER), LoadShaderFromResouce(IDS_CAMERAFRAGMENTSHADER));
	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

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
	// Vertex Array

	///////////////////////////////////////////////////////////////////
	// Uniforms

	///////////////////////////////////////////////////////////////////
	// Vertex Array
	Vector4<float> floorVertices[3];

	//Triangle
	floorVertices[0] = Vector4<float>(0.0f, -5.0f, 15.0f, 1.0f);
	floorVertices[1] = Vector4<float>(-15.0f, -5.0f, -15.0f, 1.0f);
	floorVertices[2] = Vector4<float>(15.0f, -5.0f, -15.0f, 1.0f);

	// fill buffer with informations
	VertexBufferPtr floorBuffer = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector4<float>) * 3);
	floorBuffer->VCopyFromSystemMemory(&floorVertices, 0, sizeof(Vector4<float>) * 3);

	VertexBufferAttributePtr FloorPositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(floorBuffer, ComponentDatatype::Float, 4));

	// create VertexArray
	VertexArrayPtr FloorVertexArray = contextOGL->VCreateVertexArray();

	// connect buffer with location in shader
	FloorVertexArray->VSetAttribute(CameraShaderProgram->VGetVertexAttribute("in_Position"), FloorPositionAttribute);

	///////////////////////////////////////////////////////////////////
	// Load Model
	Vector3<float> Position = Vector3<float>(0.0f, 0.0f, -5.0f);
	Vector3<float> LookAt = Vector3<float>(0.0f, 0.0f, 0.0f);
	Vector3<float> UpVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	// Game object and third Person Camera Test
	GameObject plane = GameObject(Vector3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(0.0f, 1.0f, 0.0f), Vector3<float>(0.0f, 1.0f, 0.0f), Vector3<float>(0.2f, 0.2f, 0.2f));
	plane.setMesh("../Data/SU-27/Su-27_Flanker.obj", Vector3<float>(0.0f, 0.0f, 1.0f), CameraShaderProgram, contextOGL, BufferHint::StaticDraw);
	plane.setRotation(0.0f, M_PI + M_PI / 2, M_PI);
	//plane.setRotation(0.0f, 0.0f, 0.0f);

	ThirdPersonQuaternionCamera camera2 = ThirdPersonQuaternionCamera(Position, plane.getPosition(), UpVector, windowOGL->VGetWidth(), windowOGL->VGetHeight());
	camera2.SetClippingPlanes(0.1, 10000.0);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	//ShaderProgram->VSetUniform("u_color", ColorRGB(1.0f, 1.0f, 0.0f));

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.DepthTest.Enabled = true;
	renderState.RasterizationMode = RasterizationMode::Line;

	RenderState renderState2;
	renderState2.FaceCulling.Enabled = false;
	renderState2.DepthTest.Enabled = true;
	renderState2.RasterizationMode = RasterizationMode::Fill;

	///////////////////////////////////////////////////////////////////
	// Gameloop
	BasicTimer timer;

	Matrix3D<float> Identity = Matrix3D<float>();
	Matrix3D<float> negativeIdentity = Matrix3D<float>();
	negativeIdentity._11 = negativeIdentity._22 = negativeIdentity._33 = -1.0;

	Vector2<long> lastCursorPosition;

	while (!windowOGL->VShouldClose())
	{
		// Clear Backbuffer to our ClearState
		contextOGL->VClear(clearState);

		contextOGL->VSetViewport(Viewport(0, 0, windowOGL->VGetWidth(), windowOGL->VGetHeight()));

		// Draw floor triangle
		CameraShaderProgram->VSetUniform("u_ModelViewProj", (Matrix4x4<float>)camera2.CalculateWorldViewProjection(Identity));

		CameraShaderProgram->VSetUniform("u_color", ColorRGB(1.0f, 0.0f, 1.0f));
		contextOGL->VDraw(PrimitiveType::Triangles, 0, 3, FloorVertexArray, CameraShaderProgram, renderState2);

		// Ceiling floor triangle
		CameraShaderProgram->VSetUniform("u_ModelViewProj", (Matrix4x4<float>)camera2.CalculateWorldViewProjection(negativeIdentity));

		CameraShaderProgram->VSetUniform("u_color", ColorRGB(1.0f, 0.5f, 0.5f));
		contextOGL->VDraw(PrimitiveType::Triangles, 0, 3, FloorVertexArray, CameraShaderProgram, renderState2);

		// Draw plane
		CameraShaderProgram->VSetUniform("u_ModelViewProj", (Matrix4x4<float>)camera2.CalculateWorldViewProjection(plane.getWorldMatrix()));

		CameraShaderProgram->VSetUniform("u_color", ColorRGB(1.0f, 1.0f, 0.0f));
		contextOGL->VDraw(PrimitiveType::Triangles, 0, plane.getMeshAttribute().Indices->Size(), plane.getVertexArrayPtr(), CameraShaderProgram, renderState);

		contextOGL->VSwapBuffers();
		windowOGL->VPollWindowEvents();
		keyboard->VUpdate();
		mouse->VUpdate();
		timer.Update();

		planeUpdate(&plane, mouse, keyboard, timer, windowOGL, lastCursorPosition, &camera2);
		plane.updateThirdPersonCamera(&camera2, -0.01f * moveVec.x, -0.01f * moveVec.y);
	}
	return 0;
}