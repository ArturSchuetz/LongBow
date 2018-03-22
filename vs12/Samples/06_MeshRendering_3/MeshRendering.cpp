#include "BowRenderer.h"
#include "BowResources.h"
#include "BowInput.h"

#include "FirstPersonCamera.h"

#include <cstdint>
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

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
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(800, 600, "Mesh Rendering Sample", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}
	RenderContextPtr contextOGL = windowOGL->VGetContext();
	ShaderProgramPtr shaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));
	ShaderProgramPtr bumpMapShaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_BUMPVERTEXSHADER), LoadShaderFromResouce(IDS_BUMPFRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// Vertex Array from Mesh

	MeshPtr mesh = MeshManager::GetInstance().Load("../Data/models/sponza/sponza.obj");
	std::vector<SubMesh*> subMeshes = mesh->GetSubMeshes();

	std::vector<std::string> files = mesh->GetMaterialFiles();

	std::map<std::string, Texture2DPtr> diffuseTextures;
	std::map<std::string, Texture2DPtr> normalTextures;
	for (unsigned int i = 0; i < files.size(); i++)
	{
		MaterialCollectionPtr materialCollection = MaterialManager::GetInstance().Load(files[i]);
		std::size_t foundPos = materialCollection->VGetName().find_last_of("/");
		std::string filePath = "";
		if (foundPos >= 0)
		{
			filePath = materialCollection->VGetName().substr(0, foundPos + 1);
		}

		unsigned char* defaultNormalColor = new unsigned char[4];
		defaultNormalColor[0] = defaultNormalColor[1] = 128;
		defaultNormalColor[2] = 255;

		std::vector<Material*> materials = materialCollection->GetMaterials();
		for (unsigned int j = 0; j < materials.size(); j++)
		{
			if (!materials[j]->diffuse_texname.empty())
			{
				ImagePtr image = ImageManager::GetInstance().Load(filePath + materials[j]->diffuse_texname);
				diffuseTextures.insert(std::pair<std::string, Texture2DPtr>(materials[j]->name, deviceOGL->VCreateTexture2D(image)));
			}
			else
			{
				Texture2DPtr texture = deviceOGL->VCreateTexture2D(Texture2DDescription(1, 1, TextureFormat::RedGreenBlue8, true));
				texture->VCopyFromSystemMemory(materials[j]->diffuse, ImageFormat::RedGreenBlue, ImageDatatype::UnsignedByte);
				diffuseTextures.insert(std::pair<std::string, Texture2DPtr>(materials[j]->name, texture));
			}

			if (!materials[j]->bump_texname.empty())
			{
				ImagePtr image = ImageManager::GetInstance().Load(filePath + materials[j]->bump_texname);
				normalTextures.insert(std::pair<std::string, Texture2DPtr>(materials[j]->name, deviceOGL->VCreateTexture2D(image)));
			}
			else
			{
				Texture2DPtr texture = deviceOGL->VCreateTexture2D(Texture2DDescription(1, 1, TextureFormat::RedGreenBlue8, true));
				texture->VCopyFromSystemMemory(defaultNormalColor, ImageFormat::RedGreenBlue, ImageDatatype::UnsignedByte);
				normalTextures.insert(std::pair<std::string, Texture2DPtr>(materials[j]->name, texture));
			}
		}

		delete[] defaultNormalColor;
	}

	MeshAttribute meshAttr = mesh->CreateAttribute("in_Position", "in_Normal", "in_Tangent", "in_Bitangent", "in_TexCoord");
	VertexArrayPtr vertexArray = contextOGL->VCreateVertexArray(meshAttr, bumpMapShaderProgram->VGetVertexAttributes(), BufferHint::StaticDraw);

	int texID = 0;
	int normalTexID = 1;

	TextureSamplerPtr sampler = deviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::LinearMipmapLinear, TextureMagnificationFilter::Linear, TextureWrap::Repeat, TextureWrap::Repeat);
	
	contextOGL->VSetTextureSampler(texID, sampler);
	contextOGL->VSetTextureSampler(normalTexID, sampler);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	bumpMapShaderProgram->VSetUniform("diffuseTex", texID);
	bumpMapShaderProgram->VSetUniform("normalTex", normalTexID);

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Camera

	Vector3<float> position = Vector3<float>(1100.0f, 170.0f, 0.0f);
	Vector3<float> lookAt = Vector3<float>(0.0f, 0.0f, 0.0f);
	Vector3<float> upVector = Vector3<float>(0.0f, 1.0f, 0.0f);

	FirstPersonCamera camera = FirstPersonCamera(position, lookAt, upVector, windowOGL->VGetWidth(), windowOGL->VGetHeight());
	camera.SetClippingPlanes(10.0, 10000.0);

	///////////////////////////////////////////////////////////////////
	// Input

	KeyboardPtr keyboard = InputDeviceManager::GetInstance().CreateKeyboardObject(windowOGL);
	if (keyboard == nullptr)
	{
		return false;
	}

	MousePtr mouse = InputDeviceManager::GetInstance().CreateMouseObject(windowOGL);
	if (mouse == nullptr)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.RasterizationMode = RasterizationMode::Fill;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	Matrix3D<float> worldMat;
	worldMat.Translate(Vector3<float>(0.0f, 0.0f, 0.0f));

	BasicTimer timer;
	float m_moveSpeed;
	Vector2<long> lastCursorPosition;

	while (!windowOGL->VShouldClose())
	{
		// =======================================================
		// Render Frame

		contextOGL->VClear(clearState);

		contextOGL->VSetViewport(Viewport(0, 0, windowOGL->VGetWidth(), windowOGL->VGetHeight()));
		camera.SetResolution(windowOGL->VGetWidth(), windowOGL->VGetHeight());

		bumpMapShaderProgram->VSetUniform("u_ModelView", (Matrix4x4<float>)camera.CalculateWorldView(worldMat));
		bumpMapShaderProgram->VSetUniform("u_View", (Matrix4x4<float>)camera.CalculateView());
		bumpMapShaderProgram->VSetUniform("u_Proj", (Matrix4x4<float>)camera.CalculateProjection());

		for (unsigned int i = 0; i < subMeshes.size(); i++)
		{
			std::string name = subMeshes[i]->GetMaterialName();

			contextOGL->VSetTexture(texID, diffuseTextures[name]);
			contextOGL->VSetTexture(normalTexID, normalTextures[name]);

			contextOGL->VDraw(PrimitiveType::Triangles, subMeshes[i]->GetStartIndex(), subMeshes[i]->GetNumIndices(), vertexArray, bumpMapShaderProgram, renderState);
		}

		contextOGL->VSwapBuffers();

		// =======================================================

		timer.Update();

		keyboard->VUpdate();
		mouse->VUpdate();

		m_moveSpeed = 10000.0;

		if (keyboard->VIsPressed(Key::K_W))
		{
			if (keyboard->VIsPressed(Key::K_LSHIFT))
			{
				camera.MoveForward(m_moveSpeed * (float)timer.GetDelta() * 2);
			}
			else
			{
				camera.MoveForward(m_moveSpeed * (float)timer.GetDelta());
			}
		}
		
		if (keyboard->VIsPressed(Key::K_S))
		{
			camera.MoveBackward(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_D))
		{
			camera.MoveRight(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_A))
		{
			camera.MoveLeft(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_SPACE))
		{
			camera.MoveUp(m_moveSpeed * (float)timer.GetDelta());
		}

		if (keyboard->VIsPressed(Key::K_LCONTROL))
		{
			camera.MoveDown(m_moveSpeed * (float)timer.GetDelta());
		}

		if (mouse->VIsPressed(MouseButton::MOFS_BUTTON1))
		{
			windowOGL->VHideCursor();
			Vector3<long> moveVec = mouse->VGetRelativePosition();
			camera.rotate((float)moveVec.x, (float)moveVec.y);
			mouse->VSetCursorPosition(lastCursorPosition.x, lastCursorPosition.y);
		}
		else
		{
			windowOGL->VShowCursor();
		}

		lastCursorPosition = mouse->VGetAbsolutePosition();
	}
	return 0;
}