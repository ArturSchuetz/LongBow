#include "BowRenderer.h"

#include <cstdint>
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "resource.h"

using namespace Bow;
using namespace Core;
using namespace Renderer;

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
	RenderDevicePtr DeviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(API::OpenGL3x);
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

	Mesh mesh;

	// Add Positions
	VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3("in_Position", 3);
	positionsAttribute->Values[0] = Vector3<float>(0.0f, 1.0f, 0.0f);
	positionsAttribute->Values[1] = Vector3<float>(-1.0f, -1.0f, 0.0f);
	positionsAttribute->Values[2] = Vector3<float>(1.0f, -1.0f, 0.0f);

	mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

	// Add Texture Coordinates
	VertexAttributeFloatVec2 *texCoordAttribute = new VertexAttributeFloatVec2("in_TexCoord", 3);
	texCoordAttribute->Values[0] = Vector2<float>(0.5f, 1.0f);
	texCoordAttribute->Values[1] = Vector2<float>(0.0f, 0.0f);
	texCoordAttribute->Values[2] = Vector2<float>(1.0f, 0.0f);

	mesh.AddAttribute(VertexAttributePtr(texCoordAttribute));

	// Add Indices
	IndicesUnsignedShort *indices = new IndicesUnsignedShort(3);
	indices->Values[0] = 0;
	indices->Values[1] = 1;
	indices->Values[2] = 2;
	mesh.Indices = IndicesBasePtr(indices);

	VertexArrayPtr VertexArray = ContextOGL->VCreateVertexArray(mesh, ShaderProgram->VGetVertexAttributes(), BufferHint::StaticDraw);

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Camera
	Vector3<float> Position = Vector3<float>(0.0f, 0.0f, 5.0f);
	Vector3<float> LookAt = Vector3<float>(0.0f, 0.0f, 0.0f);
	Vector3<float> UpVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	Camera camera(Position, LookAt, UpVector, WindowOGL->VGetWidth(), WindowOGL->VGetHeight());

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture = DeviceOGL->VCreateTexture2DFromFile("../Data/Textures/test.jpg");
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
	renderState.DepthTest.Enabled = false;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	Core::Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 0.0f));

	BasicTimer timer;
	while (!WindowOGL->VShouldClose())
	{
		// =======================================================
		// Render Frame

		ContextOGL->VClear(clearState);
		worldMat.RotateY(0.5f * timer.GetDelta());

		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));
		camera.SetResolution(WindowOGL->VGetWidth(), WindowOGL->VGetHeight());
		ShaderProgram->VSetUniform("u_ModelViewProj", (Core::Matrix4x4<float>)camera.CalculateWorldViewProjection(worldMat));

		ContextOGL->VDraw(PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState);

		ContextOGL->VSwapBuffers();
		WindowOGL->VPollWindowEvents();
		// =======================================================

		timer.Update();
	}
	return 0;
}