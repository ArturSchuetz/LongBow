#include "BowRenderer.h"

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
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 600, "Triangle Sample", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr windowOGL2 = deviceOGL->VCreateWindow(800, 600, "Triangle Sample2", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr contextOGL = windowOGL->VGetContext();
	RenderContextPtr contextOGL2 = windowOGL2->VGetContext();

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

	// fill buffer with informations
	VertexBufferPtr positionsBuffer = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector3<float>) * 3);
	positionsBuffer->VCopyFromSystemMemory(vertices, 0, sizeof(Vector3<float>) * 3);
	VertexBufferAttributePtr positionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(positionsBuffer, ComponentDatatype::Float, 3));

	Vector4<float> colors[3];
	colors[0] = Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f);
	colors[1] = Vector4<float>(0.0f, 1.0f, 0.0f, 1.0f);
	colors[2] = Vector4<float>(0.0f, 0.0f, 1.0f, 1.0f);

	VertexBufferPtr colorBuffer = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector4<float>) * 3);
	colorBuffer->VCopyFromSystemMemory(colors, 0, sizeof(Vector4<float>) * 3);
	VertexBufferAttributePtr colorsAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(colorBuffer, ComponentDatatype::Float, 4));

	// create VertexArray
	VertexArrayPtr vertexArray = contextOGL->VCreateVertexArray();

	// connect buffer with location in shader
	if (deviceApi == RenderDeviceAPI::OpenGL3x)
	{
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("in_Position"), positionAttribute);
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("in_Color"), colorsAttribute);
	}
	else if (deviceApi == RenderDeviceAPI::DirectX12)
	{
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("POSITION0"), positionAttribute);
		vertexArray->VSetAttribute(shaderProgram->VGetVertexAttribute("COLOR0"), colorsAttribute);
	}

	colors[0] = Vector4<float>(0.0f, 0.0f, 1.0f, 1.0f);
	colors[1] = Vector4<float>(0.0f, 1.0f, 0.0f, 1.0f);
	colors[2] = Vector4<float>(1.0f, 0.0f, 1.0f, 0.0f);

	VertexBufferPtr colorBuffer2 = deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector4<float>) * 3);
	colorBuffer2->VCopyFromSystemMemory(colors, 0, sizeof(Vector4<float>) * 3);
	VertexBufferAttributePtr colorsAttribute2 = VertexBufferAttributePtr(new VertexBufferAttribute(colorBuffer2, ComponentDatatype::Float, 4));

	VertexArrayPtr vertexArray2 = contextOGL2->VCreateVertexArray();

	// connect buffer with location in shader
	if (deviceApi == RenderDeviceAPI::OpenGL3x)
	{
		vertexArray2->VSetAttribute(shaderProgram->VGetVertexAttribute("in_Position"), positionAttribute);
		vertexArray2->VSetAttribute(shaderProgram->VGetVertexAttribute("in_Color"), colorsAttribute2);
	}
	else if (deviceApi == RenderDeviceAPI::DirectX12)
	{
		vertexArray2->VSetAttribute(shaderProgram->VGetVertexAttribute("POSITION0"), positionAttribute);
		vertexArray2->VSetAttribute(shaderProgram->VGetVertexAttribute("COLOR0"), colorsAttribute2);
	}

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


		// Clear Backbuffer to our ClearState
		contextOGL2->VClear(clearState);

		contextOGL2->VSetViewport(Viewport(0, 0, windowOGL->VGetWidth(), windowOGL->VGetHeight()));
		contextOGL2->VDraw(PrimitiveType::Triangles, vertexArray2, shaderProgram, renderState);

		contextOGL2->VSwapBuffers();
	}
	return 0;
}