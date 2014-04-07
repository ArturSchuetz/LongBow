#include "BowRenderDeviceManager.h"

#include "BowClearState.h"
#include "BowRenderState.h"

#include "IBowGraphicsWindow.h"
#include "IBowRenderDevice.h"
#include "IBowRenderContext.h"

#include "IBowVertexBuffer.h"
#include "IBowVertexArray.h"
#include "IBowShaderProgram.h"

#include "IBowTexture2D.h"
#include "IBowTextureSampler.h"

#include "BowBufferHint.h"
#include "BowVertexBufferAttribute.h"

#include "BowShaderVertexAttribute.h"

#include <cstdint>
#include <windows.h>

#include "resource.h"

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
	Bow::Renderer::RenderDevicePtr DeviceOGL = Bow::Renderer::RenderDeviceManager::GetInstance().GetOrCreateDevice(Bow::Renderer::API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	Bow::Renderer::GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "Textures Sample", Bow::Renderer::WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}

	Bow::Renderer::RenderContextPtr ContextOGL = WindowOGL->VGetContext();
	Bow::Renderer::ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShader(IDS_VERTEXSHADER), LoadShader(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	Bow::Renderer::ClearState clearState;
	float cornflowerBlue[] = { 0.392f, 0.584f, 0.929f, 1.0f };
	memcpy(&clearState.Color, &cornflowerBlue, sizeof(float)* 4);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	float* vert = new float[9];
	vert[0] = -1.0f; vert[1] = 1.0f; vert[2] = 1.0f;
	vert[3] = 0.0f; vert[4] = -1.0f; vert[5] = 1.0f;
	vert[6] = 1.0f; vert[7] = 1.0f; vert[8] = 1.0f;

	// fill buffer with informations
	Bow::Renderer::VertexBufferAttributePtr	PositionAttribute = Bow::Renderer::VertexBufferAttributePtr(new Bow::Renderer::VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(Bow::Renderer::BufferHint::StaticDraw, sizeof(float)* 9), Bow::Renderer::ComponentDatatype::Float, 3));
	PositionAttribute->GetVertexBuffer()->CopyFromSystemMemory(vert, 0, sizeof(float)* 9);

	float* texcoor = new float[6];
	texcoor[0] = 0.0f; texcoor[1] = 1.0f;
	texcoor[2] = 0.5f; texcoor[3] = 0.0f;
	texcoor[4] = 1.0f; texcoor[5] = 1.0f;

	Bow::Renderer::VertexBufferAttributePtr	TextureCoordAttribute = Bow::Renderer::VertexBufferAttributePtr(new Bow::Renderer::VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(Bow::Renderer::BufferHint::StaticDraw, sizeof(float)* 6), Bow::Renderer::ComponentDatatype::Float, 2));
	TextureCoordAttribute->GetVertexBuffer()->CopyFromSystemMemory(texcoor, 0, sizeof(float)* 6);

	// create VertexArray and connect buffer with location
	Bow::Renderer::VertexArrayPtr VertexArray = ContextOGL->CreateVertexArray();
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_Position")->Location, PositionAttribute);
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Textures

	Bow::Renderer::Texture2DPtr texture = DeviceOGL->VCreateTexture2DFromFile("Data/Textures/Logo_Art_wip_6.jpg");
	Bow::Renderer::TextureSamplerPtr sampler = DeviceOGL->VCreateTexture2DSampler(Bow::Renderer::TextureMinificationFilter::Linear, Bow::Renderer::TextureMagnificationFilter::Linear, Bow::Renderer::TextureWrap::Clamp, Bow::Renderer::TextureWrap::Clamp);

	int TexID = 0;
	ContextOGL->VSetTexture(TexID, texture);
	ContextOGL->VSetTextureSampler(TexID, sampler);
	ShaderProgram->SetUniform("diffuseTex", TexID);

	///////////////////////////////////////////////////////////////////
	// RenderState

	Bow::Renderer::RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.DepthTest.Enabled = false;

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Clear Backbuffer to our ClearState
			ContextOGL->VClear(clearState);

			ContextOGL->VDraw(Bow::Renderer::PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState);

			ContextOGL->VSwapBuffers();
		}
	}
	return (int)msg.wParam;
}