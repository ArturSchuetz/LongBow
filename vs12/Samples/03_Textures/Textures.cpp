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
	RenderDeviceAPI deviceApi = RenderDeviceAPI::DirectX12;

	// Creating Render Device
	RenderDevicePtr deviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(deviceApi);
	if (deviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 600, "Textures Sample", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr contextOGL = windowOGL->VGetContext();
	ShaderProgramPtr shaderProgram;
	if (deviceApi == RenderDeviceAPI::OpenGL3x)
	{
		shaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));
	}
	else if (deviceApi == RenderDeviceAPI::DirectX12)
	{
		shaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_HLSLVERTEXSHADER), LoadShaderFromResouce(IDS_HLSLPIXELSHADER));
	}

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	float aspectRatio = static_cast<float>(800) / static_cast<float>(600);

	Vector3<float> vertices[3];
	vertices[0] = Vector3<float>(0.0f, 0.25f * aspectRatio, 0.0f);
	vertices[1] = Vector3<float>(0.25f, -0.25f * aspectRatio, 0.0f);
	vertices[2] = Vector3<float>(-0.25f, -0.25f * aspectRatio, 0.0f);

	// Create vertex position buffer and fill with informations
	VertexBufferPtr positionBuffer = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector3<float>) * 3);
	positionBuffer->VCopyFromSystemMemory(vertices, 0, sizeof(Vector3<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr positionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(positionBuffer, ComponentDatatype::Float, 3));

	Vector2<float> texcoor[3];
	texcoor[0] = Vector2<float>(0.5f, 1.0f);
	texcoor[1] = Vector2<float>(0.0f, 0.0f);
	texcoor[2] = Vector2<float>(1.0f, 0.0f);

	// Create vertex texturecoodinate buffer and fill with informations
	VertexBufferPtr textureCoordBuffer = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	textureCoordBuffer->VCopyFromSystemMemory(texcoor, 0, sizeof(Vector2<float>) * 3);

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr textureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(textureCoordBuffer, ComponentDatatype::Float, 2));

	// create VertexArray and connect attributeBuffers with location
	VertexArrayPtr vertexArray = contextOGL->VCreateVertexArray();
	if (deviceApi == RenderDeviceAPI::OpenGL3x)
	{
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("in_Position")->Location, positionAttribute);
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("in_TexCoord")->Location, textureCoordAttribute);
	}
	else if (deviceApi == RenderDeviceAPI::DirectX12)
	{
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("POSITION0"), positionAttribute);
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("TEXCOORD0"), textureCoordAttribute);
	}

	///////////////////////////////////////////////////////////////////
	// Textures

	ImagePtr image = ImageManager::GetInstance().Load("../Data/Textures/test.png");
	Texture2DPtr texture = deviceOGL->VCreateTexture2D(image);
	TextureSamplerPtr sampler = deviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Clamp, TextureWrap::Clamp);

	int textureRegister = 0;
	contextOGL->VSetTexture(textureRegister, texture);
	contextOGL->VSetTextureSampler(textureRegister, sampler);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	shaderProgram->VSetUniform("diffuseTex", textureRegister);

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
		contextOGL->VDraw(PrimitiveType::Triangles, vertexArray, shaderProgram, renderState);

		contextOGL->VSwapBuffers();
		windowOGL->VPollWindowEvents();
	}
	return 0;
}