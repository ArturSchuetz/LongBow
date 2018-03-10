#include "BowRenderer.h"
#include "BowResources.h"

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
	ShaderProgramPtr ShaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// Vertex Array from Mesh

	MeshAttribute mesh;

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

	VertexArrayPtr VertexArray = contextOGL->VCreateVertexArray(mesh, ShaderProgram->VGetVertexAttributes(), BufferHint::StaticDraw);

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Camera
	Vector3<float> Position = Vector3<float>(0.0f, 0.0f, 5.0f);
	Vector3<float> LookAt = Vector3<float>(0.0f, 0.0f, 0.0f);
	Vector3<float> UpVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	Camera camera(Position, LookAt, UpVector, windowOGL->VGetWidth(), windowOGL->VGetHeight());

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture;

	ImagePtr image = ImageManager::GetInstance().Load("../Data/Textures/test.jpg");
	if (image->GetSizeInBytes() / (image->GetHeight() * image->GetWidth()) == 3)
		texture = deviceOGL->VCreateTexture2D(image, TextureFormat::RedGreenBlue8);
	else
		texture = deviceOGL->VCreateTexture2D(image, TextureFormat::RedGreenBlueAlpha8);

	TextureSamplerPtr sampler = deviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp);

	int TexID = 0;
	contextOGL->VSetTexture(TexID, texture);
	contextOGL->VSetTextureSampler(TexID, sampler);

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

	Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 0.0f));

	BasicTimer timer;
	while (!windowOGL->VShouldClose())
	{
		// =======================================================
		// Render Frame

		contextOGL->VClear(clearState);
		worldMat.RotateY(0.5f * timer.GetDelta());

		contextOGL->VSetViewport(Viewport(0, 0, windowOGL->VGetWidth(), windowOGL->VGetHeight()));
		camera.SetResolution(windowOGL->VGetWidth(), windowOGL->VGetHeight());
		ShaderProgram->VSetUniform("u_ModelViewProj", (Matrix4x4<float>)camera.CalculateWorldViewProjection(worldMat));

		contextOGL->VDraw(PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState);

		contextOGL->VSwapBuffers();
		windowOGL->VPollWindowEvents();
		// =======================================================

		timer.Update();
	}
	return 0;
}