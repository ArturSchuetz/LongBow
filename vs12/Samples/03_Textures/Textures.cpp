#include "BowRenderer.h"

#include <cstdint>
#include <windows.h>

#include "resource.h"

using namespace Bow;
using namespace Renderer;

std::string LoadShader(int name)
{
	DWORD size = 0;
	const char* data = NULL;
	HMODULE handle = ::GetModuleHandle(NULL);
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
	HGLOBAL rcData = ::LoadResource(handle, rc);
	size = ::SizeofResource(handle, rc);
	return std::string(static_cast<const char*>(::LockResource(rcData)));
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
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShader(IDS_VERTEXSHADER), LoadShader(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	float cornflowerBlue[] = { 0.392f, 0.584f, 0.929f, 1.0f };
	memcpy(&clearState.Color, &cornflowerBlue, sizeof(float)* 4);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	Vector2<float> vertices[3];
	vertices[0] = Vector2<float>(0.0f, 1.0f);
	vertices[1] = Vector2<float>(-1.0f, -1.0f);
	vertices[2] = Vector2<float>(1.0f, -1.0f);

	// Create vertex position buffer and fill with informations
	VertexBufferPtr PositionBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	PositionBuffer->CopyFromSystemMemory(vertices, 0, sizeof(Vector2<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(PositionBuffer, ComponentDatatype::Float, 2));

	Vector2<float> texcoor[3];
	texcoor[0] = Vector2<float>(0.5f, 1.0f);
	texcoor[1] = Vector2<float>(0.0f, 0.0f);
	texcoor[2] = Vector2<float>(1.0f, 0.0f);

	// Create vertex texturecoodinate buffer and fill with informations
	VertexBufferPtr TextureCoordBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	TextureCoordBuffer->CopyFromSystemMemory(texcoor, 0, sizeof(Vector2<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr	TextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(TextureCoordBuffer, ComponentDatatype::Float, 2));

	// create VertexArray and connect attributeBuffers with location
	VertexArrayPtr VertexArray = ContextOGL->CreateVertexArray();
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_Position")->Location, PositionAttribute);
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture = DeviceOGL->VCreateTexture2DFromFile("Data/Textures/Logo_Art_wip_6.jpg");
	TextureSamplerPtr sampler = DeviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp);

	int TexID = 0;
	ContextOGL->VSetTexture(TexID, texture);
	ContextOGL->VSetTextureSampler(TexID, sampler);
	ShaderProgram->SetUniform("diffuseTex", TexID);

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.DepthTest.Enabled = false;

	while (!WindowOGL->VShouldClose())
	{
		// Clear Backbuffer to our ClearState
		ContextOGL->VClear(clearState);

		ContextOGL->VDraw(PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState);

		ContextOGL->VSwapBuffers();
	}
	return 0;
}