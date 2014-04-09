#include "BowRenderDeviceManager.h"

#include "BowClearState.h"
#include "BowRenderState.h"

#include "IBowGraphicsWindow.h"
#include "IBowRenderDevice.h"
#include "IBowRenderContext.h"

#include "IBowVertexBuffer.h"
#include "IBowVertexArray.h"
#include "IBowShaderProgram.h"

#include "BowBufferHint.h"
#include "BowVertexBufferAttribute.h"

#include "BowShaderVertexAttribute.h"

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
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "Triangle Sample", WindowType::Windowed);
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

	float* vert = new float[9];     // vertex array 
	vert[0] = -0.3f; vert[1] = 0.5f; vert[2] = -1.0f;
	vert[3] = -0.8f; vert[4] = -0.5f; vert[5] = -1.0f;
	vert[6] = 0.2f; vert[7] = -0.5f; vert[8] = -1.0f;

	// fill buffer with informations
	VertexBufferPtr PositionBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float)* 9);
	PositionBuffer->CopyFromSystemMemory(vert, 0, sizeof(float)* 9);
	
	// Define buffer as vertexShaderAttribute for shaders
	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(PositionBuffer, ComponentDatatype::Float, 3));
	
	// create VertexArray
	VertexArrayPtr VertexArray = ContextOGL->CreateVertexArray();

	// connect buffer with location in shader
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_Position")->Location, PositionAttribute);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	float rosa[] = { 1.0f, 0.0f, 1.0f };
	ShaderProgram->SetUniformVector("u_color", rosa, 3);

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
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

			ContextOGL->VDraw(PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState);

			ContextOGL->VSwapBuffers();
		}
	}
	return (int)msg.wParam;
}