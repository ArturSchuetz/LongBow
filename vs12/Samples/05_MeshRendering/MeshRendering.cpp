#include "BowRenderer.h"

#include <cstdint>
#include <windows.h>

#include "resource.h"

using namespace Bow;
using namespace Renderer;

std::string LoadShaderFromResouce(int name)
{
	HMODULE handle	= GetModuleHandle(NULL);
	HRSRC rc		= FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
	HGLOBAL rcData	= LoadResource(handle, rc);
	return std::string(static_cast<const char*>(LockResource(rcData)));
}

int main()
{
	// Creating Render Device
	RenderDevicePtr DeviceOGL		= RenderDeviceManager::GetInstance().GetOrCreateDevice(API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL		= DeviceOGL->VCreateWindow(800, 600, "Mesh Rendering Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr ContextOGL		= WindowOGL->VGetContext();
	ShaderProgramPtr shaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearBlue;
	float cornflowerBlue[] = { 0.392f, 0.584f, 0.929f, 1.0f };
	memcpy(&clearBlue.Color, &cornflowerBlue, sizeof(float)* 4);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	Vector3<float> vertices[3];
	vertices[1] = Vector3<float>(0.0f, 1.0f, 0.0f);
	vertices[2] = Vector3<float>(-1.0f, -1.0f, 0.0f);
	vertices[0] = Vector3<float>(1.0f, -1.0f, 0.0f);

	// fill buffer with informations
	VertexBufferPtr buffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector3<float>) * 3);
					buffer->CopyFromSystemMemory(vertices, sizeof(Vector3<float>) * 3);

	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(buffer, ComponentDatatype::Float, 3));

	// create VertexArray and connect buffer with location
	VertexArrayPtr	vertexArray = ContextOGL->CreateVertexArray();
					vertexArray->SetAttribute(shaderProgram->GetVertexAttribute("in_Position")->Location, PositionAttribute);
	
	///////////////////////////////////////////////////////////////////
	// Uniforms

	float rosa[] = { 1.0f, 0.0f, 1.0f };
	shaderProgram->SetUniformVector("u_color", rosa, 3);

	Camera camera(WindowOGL->VGetWidth(), WindowOGL->VGetHeight());
	camera.SetViewLookAt(Vector3<float>(0.0f, 0.0f, 1.0f), Vector3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(0.0f, 1.0f, 0.0f));

	Core::Matrix3D<float> mat = camera.CalculateWorldViewProjection(nullptr);
	shaderProgram->SetUniformMatrix("u_ModelViewProj", mat.a, 16);

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.DepthTest.Enabled = false;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	while (!WindowOGL->VShouldClose())
	{
		ContextOGL->VClear(clearBlue);

		ContextOGL->VDraw(PrimitiveType::Triangles, vertexArray, shaderProgram, renderState);

		ContextOGL->VSwapBuffers();
	}
	return 0;
}
