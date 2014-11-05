#include "BowRenderer.h"

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
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(800, 600, "Triangle Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}
	RenderContextPtr ContextOGL = WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	float cornflowerBlue[] = { 0.392f, 0.584f, 0.929f, 1.0f };
	memcpy(&clearState.Color, &cornflowerBlue, sizeof(float)* 4);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	Vector2<float> vertices[3];
	vertices[0] = Vector2<float>(-1.0f, 1.0f);
	vertices[1] = Vector2<float>(0.0f, -1.0f);
	vertices[2] = Vector2<float>(1.0f, 1.0f);

	// fill buffer with informations
	VertexBufferPtr buffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * 3);
	buffer->VCopyFromSystemMemory(vertices, 0, sizeof(Vector2<float>) * 3);

	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(buffer, ComponentDatatype::Float, 2));
	
	// create VertexArray
	VertexArrayPtr VertexArray = ContextOGL->VCreateVertexArray();

	// connect buffer with location in shader
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, PositionAttribute);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	float rosa[] = { 1.0f, 0.0f, 1.0f };
	ShaderProgram->VSetUniformVector("u_color", rosa, 3);

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
	}
	return 0;
}