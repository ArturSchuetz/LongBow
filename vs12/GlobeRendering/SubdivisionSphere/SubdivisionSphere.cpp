#include "BowRenderer.h"
#include "BowInput.h"

#include <cstdint>
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "resource.h"

enum class SourceObject : char
{
	Tetrahedron,
	Cube,
	Octahedron,
	Icosahedron
};

using namespace Bow;
using namespace Core;
using namespace Renderer;
using namespace Input;

double frameRate = 60.0;
double smoothing = 0.7;
double waitTimer = 0.0;

int g_subdivisionLevel = 0;
SourceObject g_sourceObject = (SourceObject)0;
bool g_normalize = true;

std::string LoadShaderFromResouce(int name)
{
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
	HGLOBAL rcData = LoadResource(handle, rc);
	return std::string(static_cast<const char*>(LockResource(rcData)));
}

void waitForEndOfFrame(std::chrono::duration<double, std::milli> frameduration)
{
	// =======================================================
	// Calculate and display Framerime and Framerate

	frameRate = (frameRate * smoothing) + ((1000.0 / frameduration.count()) * (1.0 - smoothing));

	waitTimer += frameduration.count();
	if (waitTimer > 250.0)
	{
		// =======================================================
		// Calculate and display Framerime and Framerate

		char buff[5];
		snprintf(buff, sizeof(buff), "%f", frameduration.count());
		std::string frametimeStr = buff;

		std::cout << "Frametime: " << frametimeStr << " ms | Framerate: " << (int)frameRate << " | Subdivisions: ";
		std::cout << g_subdivisionLevel;
		switch (g_sourceObject)
		{
		case SourceObject::Tetrahedron:
			std::cout << " (Tetrahedron)";
			break;
		case SourceObject::Cube:
			std::cout << " (Cube)";
			break;
		case SourceObject::Octahedron:
			std::cout << " (Octahedron)";
			break;
		case SourceObject::Icosahedron:
			std::cout << " (Icosahedron)";
			break;
		}
		std::cout << "\t\t\r" << std::flush;
		// ======================================================= 
		waitTimer = 0;
	}
}

int main()
{
	// Creating Render Device
	RenderDevicePtr DeviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(Renderer::API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "Subdivision Sphere", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}

	///////////////////////////////////////////////////////////////////
	// Input
	KeyboardPtr keyboard = Input::InputDeviceManager::GetInstance().CreateKeyboardObject(WindowOGL);
	if (keyboard == nullptr)
	{
		return false;
	}

	RenderContextPtr ContextOGL = WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.95f, 0.95f, 0.95f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Camera
	Vector3<float> Position = Vector3<float>(0.0f, 0.0f, 3.0f);
	Vector3<float> LookAt = Vector3<float>(0.0f, 0.0f, 0.0f);
	Vector3<float> UpVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	Camera camera(Position, LookAt, UpVector, WindowOGL->VGetWidth(), WindowOGL->VGetHeight());

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture = DeviceOGL->VCreateTexture2DFromFile("../Data/Textures/NASA/world_topo_bathy_200411_3x5400x2700.jpg");
	TextureSamplerPtr sampler = DeviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp);

	int TexID = 0;
	ContextOGL->VSetTexture(TexID, texture);
	ContextOGL->VSetTextureSampler(TexID, sampler);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	ShaderProgram->VSetUniform("diffuseTex", TexID);

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.RasterizationMode = RasterizationMode::Line;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	unsigned int currentLEvel = g_subdivisionLevel - 1;
	SourceObject currentSourceObject = (SourceObject)((int)(g_sourceObject) - 1);
	bool currentlyNormalized = !g_normalize;
	VertexArrayPtr VertexArray = VertexArrayPtr(nullptr); 

	Core::Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 0.0f));
	worldMat.RotateX(-(M_PI * 0.5f));
	worldMat.RotateZ((M_PI * 0.00f));

	BasicTimer timer;
	bool keyUpPressed = false;
	bool keyDownPressed = false;
	bool keyLeftPressed = false;
	bool keyRightPressed = false;
	bool pressedQ = false;
	bool pressedW = false;
	bool pressedE = false;
	while (!WindowOGL->VShouldClose())
	{
		auto startFrame = std::chrono::high_resolution_clock::now(); // Take time before frame

		// =======================================================
		// Handle Input

		WindowOGL->VPollWindowEvents();
		keyboard->VUpdate();
		timer.Update();

		if (keyboard->VIsPressed(Input::Key::K_UPARROW))
		{
			if (g_subdivisionLevel < 10 && !keyUpPressed)
				g_subdivisionLevel++;

			keyUpPressed = true;
		}
		else { keyUpPressed = false; }

		if (keyboard->VIsPressed(Input::Key::K_DOWNARROW))
		{
			if (g_subdivisionLevel > 0 && !keyDownPressed)
				g_subdivisionLevel--;

			keyDownPressed = true;
		}
		else { keyDownPressed = false; }

		if (keyboard->VIsPressed(Input::Key::K_LEFTARROW))
		{
			if (!keyLeftPressed && ((int)(g_sourceObject)) > (int)SourceObject::Tetrahedron)
				g_sourceObject = (SourceObject)((int)(g_sourceObject)-1);

			keyLeftPressed = true;
		}
		else { keyLeftPressed = false; }

		if (keyboard->VIsPressed(Input::Key::K_RIGHTARROW))
		{
			if (!keyRightPressed && ((int)(g_sourceObject)) < (int)SourceObject::Icosahedron)
				g_sourceObject = (SourceObject)((int)(g_sourceObject)+1);

			keyRightPressed = true;
		}
		else { keyRightPressed = false; }

		if (keyboard->VIsPressed(Input::Key::K_Q))
		{
			if (!pressedQ)
			{
				renderState.FaceCulling.Enabled = !renderState.FaceCulling.Enabled;
			}

			pressedQ = true;
		}
		else { pressedQ = false; }

		if (keyboard->VIsPressed(Input::Key::K_W))
		{
			if (!pressedW)
			{
				if (renderState.RasterizationMode == RasterizationMode::Line)
					renderState.RasterizationMode = RasterizationMode::Fill;
				else if (renderState.RasterizationMode == RasterizationMode::Fill)
					renderState.RasterizationMode = RasterizationMode::Line;
			}

			pressedW = true;
		}
		else { pressedW = false; }

		if (keyboard->VIsPressed(Input::Key::K_E))
		{
			if (!pressedE)
			{
				g_normalize = !g_normalize;
			}

			pressedE = true;
		}
		else { pressedE = false; }

		// Create new sphere if level was changed
		if (currentLEvel != g_subdivisionLevel || currentSourceObject != g_sourceObject || currentlyNormalized != g_normalize)
		{
			currentLEvel = g_subdivisionLevel;
			currentSourceObject = g_sourceObject;
			currentlyNormalized = g_normalize;
			Mesh mesh;

			switch (currentSourceObject)
			{
			case SourceObject::Tetrahedron:
				mesh = SubdivisionSphereTessellator::ComputeFromTetrahedron(currentLEvel, currentlyNormalized);
				break;
			case SourceObject::Cube:
				mesh = SubdivisionSphereTessellator::ComputeFromCube(currentLEvel, currentlyNormalized);
				break;
			case SourceObject::Octahedron:
				mesh = SubdivisionSphereTessellator::ComputeFromOctahedron(currentLEvel, currentlyNormalized);
				break;
			case SourceObject::Icosahedron:
				mesh = SubdivisionSphereTessellator::ComputeFromIcosahedron(currentLEvel, currentlyNormalized);
				break;
			}

			VertexArray = ContextOGL->VCreateVertexArray(mesh, ShaderProgram->VGetVertexAttributes(), BufferHint::StaticDraw);
		}

		// =======================================================
		// Render Frame

		ContextOGL->VClear(clearState);
		//worldMat.RotateZ(0.5f * timer.GetDelta());

		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));
		camera.SetResolution(WindowOGL->VGetWidth(), WindowOGL->VGetHeight());
		ShaderProgram->VSetUniform("u_ModelViewProj", (Core::Matrix4x4<float>)camera.CalculateWorldViewProjection(worldMat));

		ContextOGL->VDraw(PrimitiveType::Triangles, VertexArray, ShaderProgram, renderState);

		ContextOGL->VSwapBuffers();

		// =======================================================

		auto endFrame = std::chrono::high_resolution_clock::now(); // Take time after frame

		waitForEndOfFrame(endFrame - startFrame);
	}
	return 0;
}