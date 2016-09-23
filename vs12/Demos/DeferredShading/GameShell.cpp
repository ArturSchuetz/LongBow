#include "GameShell.h"
#include "resource.h"

#include <iostream>

std::string LoadShaderFromResouce(int name)
{
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
	HGLOBAL rcData = LoadResource(handle, rc);
	return std::string(static_cast<const char*>(LockResource(rcData)));
}

GameShell::GameShell()
{
	m_camera = nullptr;
}

GameShell::~GameShell()
{
	Release();
}

bool GameShell::Init(void)
{
	std::cout << "Instuctions:" << std::endl << std::endl;
	std::cout << "\tUse W,A,S,D to move and press shift to move faster." << std::endl;
	std::cout << "\tUse spacebar to move up and left control to move down." << std::endl;
	std::cout << "\tMove mouse while pressing the right mousebutton to look around." << std::endl;
	std::cout << std::endl << std::endl;

	m_sphereMesh = Core::ResourceManager::GetInstance().LoadMesh("../Data/Models/Sphere.obj");
	if (m_sphereMesh == nullptr)
	{
		return false;
	}
	// Load Scene
	m_sceneMesh = Core::ResourceManager::GetInstance().LoadMesh("../Data/Models/lost-empire/lost_empire.obj");
	if (m_sceneMesh == nullptr)
	{
		return false;
	}

	// Creating Render Device
	m_device = Renderer::RenderDeviceManager::GetInstance().GetOrCreateDevice(Renderer::API::OpenGL3x);
	if (m_device == nullptr)
	{
		return false;
	}

	// Creating Window
	m_window = m_device->VCreateWindow(800, 600, "Lost Temple", Renderer::WindowType::Windowed);
	if (m_window == nullptr)
	{
		return false;
	}
	m_contextOGL = m_window->VGetContext();

	m_ambientShaderProgram = m_device->VCreateShaderProgram(LoadShaderFromResouce(IDS_AMBIENTVERTEXSHADER) + "*/", LoadShaderFromResouce(IDS_AMBIENTFRAGMENTSHADER) + "*/");
	if (m_ambientShaderProgram == nullptr)
	{
		return false;
	}

	m_compositeShaderProgram = m_device->VCreateShaderProgram(LoadShaderFromResouce(IDS_COMPOSITEVERTEXSHADER) + "*/", LoadShaderFromResouce(IDS_COMPOSITEFRAGMENTSHADER) + "*/");
	if (m_compositeShaderProgram == nullptr)
	{
		return false;
	}

	m_fillGBufferShaderProgram = m_device->VCreateShaderProgram(LoadShaderFromResouce(IDS_FILLBUFFERVERTEXSHADER) + "*/", LoadShaderFromResouce(IDS_FILLBUFFERFRAGMENTSHADER) + "*/");
	if (m_fillGBufferShaderProgram == nullptr)
	{
		return false;
	}

	PrepareLightVolumes();
	PrepareScreenFillingQuad();
	PrepareScene();

	m_textureSampler = m_device->VCreateTexture2DSampler(Renderer::TextureMinificationFilter::Linear, Renderer::TextureMagnificationFilter::Nearest, Renderer::TextureWrap::Clamp, Renderer::TextureWrap::Clamp);

	m_contextOGL->VSetTextureSampler(0, m_textureSampler);
	m_contextOGL->VSetTextureSampler(1, m_textureSampler);
	m_contextOGL->VSetTextureSampler(2, m_textureSampler);

	///////////////////////////////////////////////////////////////////
	// RenderState

	m_renderStateLignt.FaceCulling.FrontFaceWindingOrder = Renderer::WindingOrder::Clockwise;
	m_renderStateLignt.FaceCulling.Enabled = true;
	m_renderStateLignt.DepthTest.Enabled = false;
	m_renderStateLignt.Blending.Enabled = true;
	m_renderStateLignt.Blending.DestinationRGBFactor = Renderer::DestinationBlendingFactor::One;

	m_renderStateSolid.FaceCulling.Enabled = true;
	m_renderStateSolid.DepthTest.Enabled = true;

	m_clearState.Color = Core::ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// FrameBuffer for Deferred Rendering

	m_GeometryBuffer = m_contextOGL->VCreateFramebuffer();
	AlbedoRT_Location = m_fillGBufferShaderProgram->VGetFragmentOutputLocation("out_Albedo");
	NormalRT_Location = m_fillGBufferShaderProgram->VGetFragmentOutputLocation("out_Normal");

	///////////////////////////////////////////////////////////////////
	// Input
	m_keyboard = Input::InputDeviceManager::GetInstance().CreateKeyboardObject(m_window);
	if (m_keyboard == nullptr)
	{
		return false;
	}

	m_mouse = Input::InputDeviceManager::GetInstance().CreateMouseObject(m_window);
	if (m_mouse == nullptr)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	Core::Vector3<float> Position = Core::Vector3<float>(m_sceneMesh->m_Center.x - 8.0f, m_sceneMesh->m_Center.y - 5.0f, m_sceneMesh->m_Center.z);
	Core::Vector3<float> LookAt = m_sceneMesh->m_Center + Core::Vector3<float>(0.0f, -5.0f, 10.0f);
	Core::Vector3<float> UpVector = Core::Vector3<float>(0.0f, 1.0f, 0.0f);

	m_camera = new FirstPersonCamera(Position, LookAt, UpVector, m_window->VGetWidth(), m_window->VGetHeight());

	for (unsigned int i = 0; i < 100; ++i)
	{
		m_lightPositions.push_back(Core::Vector3<float>((static_cast <float> (rand()) * 2.0f - RAND_MAX) * m_sceneMesh->m_Width / static_cast <float> (RAND_MAX), (static_cast <float> (rand()) * 2.0f - RAND_MAX) * m_sceneMesh->m_Height / static_cast <float> (RAND_MAX), (static_cast <float> (rand()) * 2.0f - RAND_MAX) * m_sceneMesh->m_Length / static_cast <float> (RAND_MAX)));
	}

	return true;
}

void GameShell::PrepareScreenFillingQuad(void)
{
	///////////////////////////////////////////////////////////////////
	// Screenfilling Quad Preperations

	float* QuadVert = new float[8];     // vertex array 
	QuadVert[0] = -1.0f; QuadVert[1] = 1.0f;
	QuadVert[2] = -1.0f; QuadVert[3] = -1.0f;
	QuadVert[4] = 1.0f; QuadVert[5] = 1.0f;
	QuadVert[6] = 1.0f; QuadVert[7] = -1.0f;

	float* QuadTexcoor = new float[8];     // vertex array 
	QuadTexcoor[0] = 0.0f; QuadTexcoor[1] = 1.0f;
	QuadTexcoor[2] = 0.0f; QuadTexcoor[3] = 0.0f;
	QuadTexcoor[4] = 1.0f; QuadTexcoor[5] = 1.0f;
	QuadTexcoor[6] = 1.0f; QuadTexcoor[7] = 0.0f;

	// fill buffer with informations
	auto QuadPositionAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(float) * 8), Renderer::ComponentDatatype::Float, 2));
	QuadPositionAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadVert, 0, sizeof(float) * 8);
	delete[] QuadVert;

	auto QuadTextureCoordAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(float) * 8), Renderer::ComponentDatatype::Float, 2));
	QuadTextureCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadTexcoor, 0, sizeof(float) * 8);
	delete[] QuadTexcoor;

	// connect buffer with location
	m_QuadVA = m_contextOGL->VCreateVertexArray();
	m_QuadVA->VSetAttribute(m_ambientShaderProgram->VGetVertexAttribute("in_Position"), QuadPositionAttribute);
	m_QuadVA->VSetAttribute(m_ambientShaderProgram->VGetVertexAttribute("in_TexCoord"), QuadTextureCoordAttribute);
}

void GameShell::PrepareLightVolumes(void)
{
	///////////////////////////////////////////////////////////////////
	// LightVolume Preperations

	Renderer::IndexBufferPtr indexBuffer;
	if (m_sphereMesh->m_Indices->Type == Core::IndicesType::UnsignedInt)
	{
		indexBuffer = m_device->VCreateIndexBuffer(Renderer::BufferHint::StaticDraw, Renderer::IndexBufferDatatype::UnsignedInt, sizeof(unsigned int) * m_sphereMesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((Core::IndicesUnsignedInt*)(m_sphereMesh->m_Indices))->Values[0]), sizeof(unsigned int) * m_sphereMesh->GetNumberOfIndices());
	}
	else if (m_sphereMesh->m_Indices->Type == Core::IndicesType::UnsignedShort)
	{
		indexBuffer = m_device->VCreateIndexBuffer(Renderer::BufferHint::StaticDraw, Renderer::IndexBufferDatatype::UnsignedShort, sizeof(unsigned short) * m_sphereMesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((Core::IndicesUnsignedShort*)(m_sphereMesh->m_Indices))->Values[0]), sizeof(unsigned short) * m_sphereMesh->GetNumberOfIndices());
	}


	// fill buffer with informations
	Renderer::VertexBufferPtr VertexBuffer = m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(Core::Vector3<float>) * m_sphereMesh->GetNumberOfVertices());
	VertexBuffer->VCopyFromSystemMemory(&(m_sphereMesh->m_Positions.at(0)), sizeof(Core::Vector3<float>) * m_sphereMesh->GetNumberOfVertices());

	Renderer::VertexBufferAttributePtr PositionAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(VertexBuffer, Renderer::ComponentDatatype::Float, 3));

	// connect buffer with location
	m_LightVA = m_contextOGL->VCreateVertexArray();
	m_LightVA->VSetIndexBuffer(indexBuffer);
	m_LightVA->VSetAttribute(m_compositeShaderProgram->VGetVertexAttribute("in_Position"), PositionAttribute);
}

void GameShell::PrepareScene(void)
{
	///////////////////////////////////////////////////////////////////
	// Create Vertex Array from Mesh (3D Models are not yet implemented)

	// fill buffer with informations
	Renderer::IndexBufferPtr indexBuffer;
	if (m_sceneMesh->m_Indices->Type == Core::IndicesType::UnsignedInt)
	{
		indexBuffer = m_device->VCreateIndexBuffer(Renderer::BufferHint::StaticDraw, Renderer::IndexBufferDatatype::UnsignedInt, sizeof(unsigned int) * m_sceneMesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((Core::IndicesUnsignedInt*)(m_sceneMesh->m_Indices))->Values[0]), sizeof(unsigned int) * m_sceneMesh->GetNumberOfIndices());
	}
	else if (m_sceneMesh->m_Indices->Type == Core::IndicesType::UnsignedShort)
	{
		indexBuffer = m_device->VCreateIndexBuffer(Renderer::BufferHint::StaticDraw, Renderer::IndexBufferDatatype::UnsignedShort, sizeof(unsigned short) * m_sceneMesh->GetNumberOfIndices());
		indexBuffer->VCopyFromSystemMemory(&(((Core::IndicesUnsignedShort*)(m_sceneMesh->m_Indices))->Values[0]), sizeof(unsigned short) * m_sceneMesh->GetNumberOfIndices());
	}

	/////////////////////////
	// POSITIONS

	Renderer::VertexBufferPtr VertexBuffer = m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(Core::Vector3<float>) * m_sceneMesh->GetNumberOfVertices());
	VertexBuffer->VCopyFromSystemMemory(&(m_sceneMesh->m_Positions.at(0)), sizeof(Core::Vector3<float>) * m_sceneMesh->GetNumberOfVertices());

	Renderer::VertexBufferAttributePtr PositionAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(VertexBuffer, Renderer::ComponentDatatype::Float, 3));

	/////////////////////////
	// NORMALS

	Renderer::VertexBufferPtr NormalBuffer = m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(Core::Vector3<float>) * m_sceneMesh->GetNumberOfVertices());
	NormalBuffer->VCopyFromSystemMemory(&(m_sceneMesh->m_Normals.at(0)), sizeof(Core::Vector3<float>) * m_sceneMesh->GetNumberOfVertices());

	Renderer::VertexBufferAttributePtr NormalAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(NormalBuffer, Renderer::ComponentDatatype::Float, 3));

	/////////////////////////
	// TEXTURE COORDINATES

	// Create vertex texturecoodinate buffer and fill with informations
	Renderer::VertexBufferPtr TextureCoordBuffer = m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(Core::Vector2<float>) * m_sceneMesh->GetNumberOfVertices());
	TextureCoordBuffer->VCopyFromSystemMemory(&(m_sceneMesh->m_TextureCoords.at(0)), 0, sizeof(Core::Vector2<float>) * m_sceneMesh->GetNumberOfVertices());

	// Define buffer as vertexattribute for shaders
	Renderer::VertexBufferAttributePtr TextureCoordAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(TextureCoordBuffer, Renderer::ComponentDatatype::Float, 2));

	// create VertexArray and connect buffer with location in shader
	m_sceneVA = m_contextOGL->VCreateVertexArray();
	m_sceneVA->VSetIndexBuffer(indexBuffer);
	m_sceneVA->VSetAttribute(m_fillGBufferShaderProgram->VGetVertexAttribute("in_Position"), PositionAttribute);
	m_sceneVA->VSetAttribute(m_fillGBufferShaderProgram->VGetVertexAttribute("in_Normal"), NormalAttribute);
	m_sceneVA->VSetAttribute(m_fillGBufferShaderProgram->VGetVertexAttribute("in_TexCoord"), TextureCoordAttribute);
}

void GameShell::Release(void)
{
	if (m_camera)
	{
		delete m_camera;
	}
}

void GameShell::Update(DWORD deltaTime)
{
	m_window->VPollWindowEvents();
	m_keyboard->VUpdate();
	m_mouse->VUpdate();

	if (m_keyboard->VIsPressed(Input::Key::K_W) || m_keyboard->VIsPressed(Input::Key::K_UP))
	{
		if (m_keyboard->VIsPressed(Input::Key::K_LSHIFT))
		{
			m_camera->MoveForward(deltaTime * 2);
		}
		else
		{
			m_camera->MoveForward(deltaTime);
		}
	}

	if (m_keyboard->VIsPressed(Input::Key::K_S) || m_keyboard->VIsPressed(Input::Key::K_DOWN))
	{
		m_camera->MoveBackward(deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_D) || m_keyboard->VIsPressed(Input::Key::K_RIGHT))
	{
		m_camera->MoveRight(deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_A) || m_keyboard->VIsPressed(Input::Key::K_LEFT))
	{
		m_camera->MoveLeft(deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_SPACE))
	{
		m_camera->MoveUp(deltaTime);
	}

	if (m_keyboard->VIsPressed(Input::Key::K_LCONTROL))
	{
		m_camera->MoveDown(deltaTime);
	}

	if (m_mouse->VIsPressed(Input::MouseButton::MOFS_BUTTON1))
	{
		m_window->VHideCursor();
		Core::Vector3<long> moveVec = m_mouse->VGetRelativePosition();
		m_camera->rotate((float)moveVec.x, (float)moveVec.y);
		m_mouse->VSetCursorPosition(m_lastCursorPosition.x, m_lastCursorPosition.y);
	}
	else
	{
		m_window->VShowCursor();
	}

	m_lastCursorPosition = m_mouse->VGetAbsolutePosition();
}


void GameShell::Render(void)
{
	// prepare camera
	m_camera->SetResolution(m_window->VGetWidth(), m_window->VGetHeight());
	Core::Matrix4x4<float> matProjInv = (Core::Matrix4x4<float>)m_camera->CalculateProjection().Inverse();
	Core::Matrix4x4<float> matViewProj = (Core::Matrix4x4<float>)m_camera->CalculateViewProjection();
	Core::Matrix3D<float> matView = (Core::Matrix3D<float>)m_camera->CalculateView();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fill G-Buffer
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Change Gbuffer size if resolution changes
	if (GBuffer_Width != m_window->VGetWidth() || GBuffer_Height != m_window->VGetHeight())
	{
		m_AlbedoBuffer = m_device->VCreateTexture2D(Renderer::Texture2DDescription(m_window->VGetWidth(), m_window->VGetHeight(), Renderer::TextureFormat::RedGreenBlue8));
		m_GeometryBuffer->VSetColorAttachment(AlbedoRT_Location, m_AlbedoBuffer);

		m_DepthBuffer = m_device->VCreateTexture2D(Renderer::Texture2DDescription(m_window->VGetWidth(), m_window->VGetHeight(), Renderer::TextureFormat::Depth24));
		m_GeometryBuffer->VSetDepthAttachment(m_DepthBuffer);

		m_NormalBuffer = m_device->VCreateTexture2D(Renderer::Texture2DDescription(m_window->VGetWidth(), m_window->VGetHeight(), Renderer::TextureFormat::RedGreenBlue16f));
		m_GeometryBuffer->VSetColorAttachment(NormalRT_Location, m_NormalBuffer);

		GBuffer_Height = m_window->VGetHeight();
		GBuffer_Width = m_window->VGetWidth();
	}

	// prepare context
	m_contextOGL->VSetFramebuffer(m_GeometryBuffer);
	m_contextOGL->VSetViewport(Renderer::Viewport(0, 0, m_window->VGetWidth(), m_window->VGetHeight()));
	m_contextOGL->VClear(m_clearState);

	// prepare shader
	m_fillGBufferShaderProgram->VSetUniform("u_ViewProj", matViewProj);
	m_fillGBufferShaderProgram->VSetUniform("u_View", matView);
	m_fillGBufferShaderProgram->VSetUniform("diffuseTex", 0);

	// render all submeshes
	for (unsigned int i = 0; i < m_sceneMesh->GetNumberOfSubmeshes(); ++i)
	{
		// set texture for submesh
		if (m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetSizeInBytes() / (m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetHeight() * m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetWidth()) == 3)
		{
			m_contextOGL->VSetTexture(0, m_device->VCreateTexture2D(m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture, Renderer::TextureFormat::RedGreenBlue8, true));
		}
		else if (m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetSizeInBytes() / (m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetHeight() * m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture->GetWidth()) == 4)
		{
			m_contextOGL->VSetTexture(0, m_device->VCreateTexture2D(m_sceneMesh->m_Materials[m_sceneMesh->m_SubMeshes[i].MaterialID].diffuseTexture, Renderer::TextureFormat::RedGreenBlueAlpha8, true));
		}

		// drawcall
		m_contextOGL->VDraw(Renderer::PrimitiveType::Triangles, m_sceneMesh->m_SubMeshes[i].StartIndex, m_sceneMesh->m_SubMeshes[i].NumIndices, m_sceneVA, m_fillGBufferShaderProgram, m_renderStateSolid);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Lignting
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_contextOGL->VSetFramebuffer(nullptr);
	m_contextOGL->VSetViewport(Renderer::Viewport(0, 0, m_window->VGetWidth(), m_window->VGetHeight()));
	m_contextOGL->VClear(m_clearState);

	m_contextOGL->VSetTexture(0, m_AlbedoBuffer);
	m_contextOGL->VSetTexture(1, m_NormalBuffer);
	m_contextOGL->VSetTexture(2, m_DepthBuffer);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render Ambient
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_ambientShaderProgram->VSetUniform("u_ProjInv", matProjInv);
	m_ambientShaderProgram->VSetUniform("albedoTex", 0);
	m_ambientShaderProgram->VSetUniform("normalTex", 1);
	m_ambientShaderProgram->VSetUniform("depthTex", 2);

	m_contextOGL->VDraw(Renderer::PrimitiveType::TriangleStrip, m_QuadVA, m_ambientShaderProgram, m_renderStateSolid);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render Lights
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const float radius = 35.0f;
	const float invRadius = 1.0f / radius;
	
	m_compositeShaderProgram->VSetUniform("u_ProjInv", matProjInv);
	m_compositeShaderProgram->VSetUniform("u_ViewProj", matViewProj);
	m_compositeShaderProgram->VSetUniform("u_View", matView);
	m_compositeShaderProgram->VSetUniform("u_CanvasSize", Core::Vector2<float>(m_window->VGetWidth(), m_window->VGetHeight()));
	m_compositeShaderProgram->VSetUniform("albedoTex", 0);
	m_compositeShaderProgram->VSetUniform("normalTex", 1);
	m_compositeShaderProgram->VSetUniform("depthTex", 2);

	for (unsigned int i = 0; i < m_lightPositions.size(); ++i)
	{
		// Compute z-bounds
		Core::Vector3<float> lViewPos = m_lightPositions[i];
		float z1 = lViewPos.z + radius;

		if (z1 > 0.5f){
			m_compositeShaderProgram->VSetUniform("u_lightPos", (Core::Vector3<float>)lViewPos);
			m_compositeShaderProgram->VSetUniform("u_radius", radius);
			m_compositeShaderProgram->VSetUniform("u_invRadius", invRadius);
	
			for (unsigned int i = 0; i < m_sphereMesh->GetNumberOfSubmeshes(); ++i)
			{
				m_contextOGL->VDraw(Renderer::PrimitiveType::Triangles, m_sphereMesh->m_SubMeshes[i].StartIndex, m_sphereMesh->m_SubMeshes[i].NumIndices, m_LightVA, m_compositeShaderProgram, m_renderStateLignt);
			}
		}
	}

	m_contextOGL->VSwapBuffers(true);
}
