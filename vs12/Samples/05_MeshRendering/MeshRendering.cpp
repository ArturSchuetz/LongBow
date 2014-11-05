#include "BowRenderer.h"
#include "BowCore.h"

#include <cstdint>
#include <windows.h>

#include "resource.h"

using namespace Bow;
using namespace Core;
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
	///////////////////////////////////////////////////////////////////
	// Load Mesh

	LOG_TRACE("Loading Modeldata...");
	MeshPtr mesh = ResourceManager::GetInstance().LoadMesh("./IronManNoRig.obj");

	if (mesh == nullptr)
	{
		return -1;
	}

	///////////////////////////////////////////////////////////////////
	// Creating Render Device
	RenderDevicePtr DeviceOGL		= RenderDeviceManager::GetInstance().GetOrCreateDevice(API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return -1;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL		= DeviceOGL->VCreateWindow(450, 600, "Mesh Rendering Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return -1;
	}

	RenderContextPtr ContextOGL		= WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearBlue;
	float cornflowerBlue[] = { 0.392f, 0.584f, 0.929f, 1.0f };
	memcpy(&clearBlue.Color, &cornflowerBlue, sizeof(float)* 4);

	///////////////////////////////////////////////////////////////////
	// Create Vertexa Array from Mesh

	// fill buffer with informations
	IndexBufferPtr indexBuffer; 
	if (mesh->Indices->Type == IndicesType::UnsignedInt)
	{
		indexBuffer = DeviceOGL->VCreateIndexBuffer(BufferHint::StaticDraw, IndexBufferDatatype::UnsignedInt, sizeof(unsigned int) * mesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((IndicesUnsignedInt*)(mesh->Indices))->Values[0]), sizeof(unsigned int) * mesh->GetNumberOfIndices());
	}
	else if (mesh->Indices->Type == IndicesType::UnsignedShort)
	{
		indexBuffer = DeviceOGL->VCreateIndexBuffer(BufferHint::StaticDraw, IndexBufferDatatype::UnsignedShort, sizeof(unsigned short) * mesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((IndicesUnsignedShort*)(mesh->Indices))->Values[0]), sizeof(unsigned short) * mesh->GetNumberOfIndices());
	}

	VertexBufferPtr VertexBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector3<float>) * mesh->GetNumberOfVertices());
	VertexBuffer->VCopyFromSystemMemory(mesh->Positions, sizeof(Vector3<float>) * mesh->GetNumberOfVertices());

	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(VertexBuffer, ComponentDatatype::Float, 3));

	// create VertexArray
	VertexArrayPtr VertexArray = ContextOGL->VCreateVertexArray();

	// connect buffer with location in shader
	VertexArray->VSetIndexBuffer(indexBuffer);
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, PositionAttribute);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	ShaderProgram->VSetUniform("u_color", Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f));

	Camera camera(WindowOGL->VGetWidth(), WindowOGL->VGetHeight());
	Vector3<float> Position = Vector3<float>(258.634399f, 126.081482f, 258.634399f);
	Vector3<float> LookAt = Vector3<float>(-0.873367310f, 126.081482f, 0.599601746f);
	Vector3<float> UpVector = Vector3<float>(0.0f, 1.0f, 0.0f);
	camera.SetViewLookAt(Position, LookAt, UpVector);

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.DepthTest.Enabled = true;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	Core::Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 35.0f));

	while (!WindowOGL->VShouldClose())
	{
		ContextOGL->VClear(clearBlue);
		
		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));

		camera.SetResolution(WindowOGL->VGetWidth(), WindowOGL->VGetHeight());

		ShaderProgram->VSetUniform("u_ModelViewProj", (Core::Matrix3D<float>)camera.CalculateWorldViewProjection(worldMat));

		for (unsigned int i = 0; i < mesh->GetNumberOfSubmeshes(); ++i)
		{
			ShaderProgram->VSetUniform("u_color", mesh->Materials[mesh->SubMeshes[i].MaterialID].diffuse);
			ContextOGL->VDraw(PrimitiveType::Triangles, mesh->SubMeshes[i].StartIndex, mesh->SubMeshes[i].TriangleCount * 3, VertexArray, ShaderProgram, renderState);
		}

		ContextOGL->VSwapBuffers();
	}
	return 0;
}
