#include "BowRenderer.h"
#include "BowResources.h"

#include <cstdint>
#include <windows.h>

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
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 800, "Textures Sample", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr contextOGL = windowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

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
	VertexBufferPtr PositionBuffer = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	PositionBuffer->VCopyFromSystemMemory(vertices, 0, sizeof(Vector2<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(PositionBuffer, ComponentDatatype::Float, 2));

	Vector2<float> texcoor[3];
	texcoor[0] = Vector2<float>(0.5f, 1.0f);
	texcoor[1] = Vector2<float>(0.0f, 0.0f);
	texcoor[2] = Vector2<float>(1.0f, 0.0f);

	// Create vertex texturecoodinate buffer and fill with informations
	VertexBufferPtr TextureCoordBuffer = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	TextureCoordBuffer->VCopyFromSystemMemory(texcoor, 0, sizeof(Vector2<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr TextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(TextureCoordBuffer, ComponentDatatype::Float, 2));

	// create VertexArray and connect attributeBuffers with location
	VertexArrayPtr VertexArray = contextOGL->VCreateVertexArray();
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, PositionAttribute);
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture;

	ImagePtr image = ImageManager::GetInstance().Load("../Data/Textures/test.png");
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

	while (!windowOGL->VShouldClose())
	{
		// Clear Backbuffer to our ClearState
		contextOGL->VClear(clearState);

		contextOGL->VSetViewport(Viewport(0, 0, windowOGL->VGetWidth(), windowOGL->VGetHeight()));
		contextOGL->VDraw(PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState);

		contextOGL->VSwapBuffers();
		windowOGL->VPollWindowEvents();
	}
	return 0;
}