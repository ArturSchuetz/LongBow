#include "BowRenderer.h"
#include "BowBitmap.h"

#include <cstdint>
#include <windows.h>

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
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 800, "Textures Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr ContextOGL = WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	Vector2<float> vertices[3];
	vertices[0] = Vector2<float>(0.0f, 1.0f);
	vertices[1] = Vector2<float>(-1.0f, -1.0f);
	vertices[2] = Vector2<float>(1.0f, -1.0f);

	// Create vertex position buffer and fill with informations
	VertexBufferPtr PositionBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	PositionBuffer->VCopyFromSystemMemory(vertices, 0, sizeof(Vector2<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(PositionBuffer, ComponentDatatype::Float, 2));

	Vector2<float> texcoor[3];
	texcoor[0] = Vector2<float>(0.5f, 1.0f);
	texcoor[1] = Vector2<float>(0.0f, 0.0f);
	texcoor[2] = Vector2<float>(1.0f, 0.0f);

	// Create vertex texturecoodinate buffer and fill with informations
	VertexBufferPtr TextureCoordBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	TextureCoordBuffer->VCopyFromSystemMemory(texcoor, 0, sizeof(Vector2<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr TextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(TextureCoordBuffer, ComponentDatatype::Float, 2));

	// create VertexArray and connect attributeBuffers with location
	VertexArrayPtr VertexArray = ContextOGL->VCreateVertexArray();
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, PositionAttribute);
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture;

	Core::Bitmap bitmap;
	bitmap.LoadFile("../Data/Textures/test.jpg");

	if (bitmap.GetSizeInBytes() / (bitmap.GetHeight() * bitmap.GetWidth()) == 3)
		texture = DeviceOGL->VCreateTexture2D(&bitmap, Renderer::TextureFormat::RedGreenBlue8);
	else
		texture = DeviceOGL->VCreateTexture2D(&bitmap, Renderer::TextureFormat::RedGreenBlueAlpha8);

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

	while (!WindowOGL->VShouldClose())
	{
		// Clear Backbuffer to our ClearState
		ContextOGL->VClear(clearState);

		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));
		ContextOGL->VDraw(PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState);

		ContextOGL->VSwapBuffers();
		WindowOGL->VPollWindowEvents();
	}
	return 0;
}