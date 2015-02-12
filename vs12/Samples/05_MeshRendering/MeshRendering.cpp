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
	MeshPtr mesh = ResourceManager::GetInstance().LoadMesh("../Data/Models/lost-empire/lost_empire.obj");
	if (mesh == nullptr)
	{
		return -1;
	}

	///////////////////////////////////////////////////////////////////
	// Creating Render Device
	RenderDevicePtr DeviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return -1;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL	= DeviceOGL->VCreateWindow(800, 600, "Mesh Rendering Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return -1;
	}

	RenderContextPtr ContextOGL	= WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Create Vertexa Array from Mesh
	
	// fill buffer with informations
	IndexBufferPtr indexBuffer; 
	if (mesh->m_Indices->Type == IndicesType::UnsignedInt)
	{
		indexBuffer = DeviceOGL->VCreateIndexBuffer(BufferHint::StaticDraw, IndexBufferDatatype::UnsignedInt, sizeof(unsigned int) * mesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((IndicesUnsignedInt*)(mesh->m_Indices))->Values[0]), sizeof(unsigned int) * mesh->GetNumberOfIndices());
	}
	else if (mesh->m_Indices->Type == IndicesType::UnsignedShort)
	{
		indexBuffer = DeviceOGL->VCreateIndexBuffer(BufferHint::StaticDraw, IndexBufferDatatype::UnsignedShort, sizeof(unsigned short) * mesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((IndicesUnsignedShort*)(mesh->m_Indices))->Values[0]), sizeof(unsigned short) * mesh->GetNumberOfIndices());
	}

	/////////////////////////
	// POSITIONS
	VertexBufferPtr VertexBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector3<float>) * mesh->GetNumberOfVertices());
	VertexBuffer->VCopyFromSystemMemory(&(mesh->m_Positions.at(0)), sizeof(Vector3<float>) * mesh->GetNumberOfVertices());

	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(VertexBuffer, ComponentDatatype::Float, 3));

	/////////////////////////
	// TEXTURE COORDINATES
	// Create vertex texturecoodinate buffer and fill with informations
	VertexBufferPtr TextureCoordBuffer = DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(Vector2<float>) * mesh->GetNumberOfVertices());
	TextureCoordBuffer->VCopyFromSystemMemory(&(mesh->m_TextureCoords.at(0)), 0, sizeof(Vector2<float>) * mesh->GetNumberOfVertices());

	// Define buffer as vertexattribute for shaders
	VertexBufferAttributePtr TextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(TextureCoordBuffer, ComponentDatatype::Float, 2));


	// create VertexArray
	VertexArrayPtr VertexArray = ContextOGL->VCreateVertexArray();
	
	// connect buffer with location in shader
	VertexArray->VSetIndexBuffer(indexBuffer);
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, PositionAttribute);
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute);


	///////////////////////////////////////////////////////////////////
	// Uniforms

	Vector3<float> Position = Vector3<float>(mesh->m_Center.x - 8.0f, mesh->m_Center.y - 1.5f, mesh->m_Center.z - 16.0f);
	Vector3<float> LookAt = mesh->m_Center + Vector3<float>(0.0f, -5.0f, 10.0f);
	Vector3<float> UpVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	Camera camera(Position, LookAt, UpVector, WindowOGL->VGetWidth(), WindowOGL->VGetHeight());

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderStateSolid;
	renderStateSolid.FaceCulling.Enabled = true;
	renderStateSolid.DepthTest.Enabled = true;

	RenderState renderStateTransparent;
	renderStateTransparent.FaceCulling.Enabled = true;
	renderStateTransparent.DepthTest.Enabled = true;
	renderStateTransparent.Blending.Enabled = true;
	renderStateTransparent.Blending.DestinationRGBFactor = DestinationBlendingFactor::OneMinusSourceAlpha;
	renderStateTransparent.Blending.SourceRGBFactor = SourceBlendingFactor::SourceAlpha;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	Core::Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 35.0f));

	// Textures
	int TexID = 0;
	TextureSamplerPtr sampler = DeviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Nearest, TextureWrap::Clamp, TextureWrap::Clamp);

	while (!WindowOGL->VShouldClose())
	{
		ContextOGL->VClear(clearState);
		
		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));

		camera.SetResolution(WindowOGL->VGetWidth(), WindowOGL->VGetHeight());

		ShaderProgram->VSetUniform("u_ModelViewProj", (Core::Matrix4x4<float>)camera.CalculateWorldViewProjection(worldMat));

		for (unsigned int i = 0; i < mesh->GetNumberOfSubmeshes(); ++i)
		{
			if (mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetSizeInBytes() / (mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetHeight() * mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetWidth()) == 3)
			{
				ContextOGL->VSetTexture(TexID, DeviceOGL->VCreateTexture2D(mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture, TextureFormat::RedGreenBlue8, true));

				ContextOGL->VSetTextureSampler(TexID, sampler);
				ShaderProgram->VSetUniform("diffuseTex", TexID);
				ContextOGL->VDraw(PrimitiveType::Triangles, mesh->m_SubMeshes[i].StartIndex, mesh->m_SubMeshes[i].NumIndices, VertexArray, ShaderProgram, renderStateSolid);
			}

		}

		// Transparent Objects (without sorting so far)
		for (unsigned int i = 0; i < mesh->GetNumberOfSubmeshes(); ++i)
		{
			if (mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetSizeInBytes() / (mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetHeight() * mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetWidth()) == 4)
			{
				ContextOGL->VSetTexture(TexID, DeviceOGL->VCreateTexture2D(mesh->m_Materials[mesh->m_SubMeshes[i].MaterialID].diffuseTexture, TextureFormat::RedGreenBlueAlpha8, true));

				ContextOGL->VSetTextureSampler(TexID, sampler);
				ShaderProgram->VSetUniform("diffuseTex", TexID);
				ContextOGL->VDraw(PrimitiveType::Triangles, mesh->m_SubMeshes[i].StartIndex, mesh->m_SubMeshes[i].NumIndices, VertexArray, ShaderProgram, renderStateTransparent);
			}
		}

		ContextOGL->VSwapBuffers();
		WindowOGL->VPollWindowEvents();
	}
	return 0;
}
