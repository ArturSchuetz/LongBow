#include "GameShell.h"
#include "resource.h"

const float Pi = 3.141592f;
const int g_diffuseTextureID = 0;
const int g_nightTextureID = 1;

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
	// Load Scene
	m_sceneMesh = Core::ResourceManager::GetInstance().LoadMesh("../Data/Models/HighPolySphere.obj");
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
	m_window = m_device->VCreateWindow(600, 600, "Atmosphere", Renderer::WindowType::Windowed);
	if (m_window == nullptr)
	{
		return false;
	}
	m_contextOGL = m_window->VGetContext();

	// Compile and link shader programm
	m_GroundShaderProgram = m_device->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXGROUNDSHADER), LoadShaderFromResouce(IDS_FRAGMENTGROUNDSHADER));
	if (m_GroundShaderProgram == nullptr)
	{
		return false;
	}

	m_SkyShaderProgram = m_device->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSKYSHADER), LoadShaderFromResouce(IDS_FRAGMENTSKYSHADER));
	if (m_SkyShaderProgram == nullptr)
	{
		return false;
	}

	// Load textures from filepath for earth model
	m_diffuse_texture = m_device->VCreateTexture2DFromFile("../Data/Textures/NASA/world.topo.200412.3x5400x2700.jpg", false);
	if (m_diffuse_texture == nullptr)
	{
		return false;
	}

	m_night_texture = m_device->VCreateTexture2DFromFile("../Data/Textures/NASA/land_ocean_ice_lights_2048.jpg", false);
	if (m_night_texture == nullptr)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// Create vertex array from mesh for rendering

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

	// POSITIONS
	Renderer::VertexBufferPtr VertexBuffer = m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(Core::Vector3<float>) * m_sceneMesh->GetNumberOfVertices());
	VertexBuffer->VCopyFromSystemMemory(&(m_sceneMesh->m_Positions.at(0)), sizeof(Core::Vector3<float>) * m_sceneMesh->GetNumberOfVertices());

	Renderer::VertexBufferAttributePtr PositionAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(VertexBuffer, Renderer::ComponentDatatype::Float, 3));

	// TEXTURE COORDINATES
	// Create vertex texturecoodinate buffer and fill with informations
	Renderer::VertexBufferPtr TextureCoordBuffer = m_device->VCreateVertexBuffer(Renderer::BufferHint::StaticDraw, sizeof(Core::Vector2<float>) * m_sceneMesh->GetNumberOfVertices());
	TextureCoordBuffer->VCopyFromSystemMemory(&(m_sceneMesh->m_TextureCoords.at(0)), 0, sizeof(Core::Vector2<float>) * m_sceneMesh->GetNumberOfVertices());

	// Define buffer as vertexattribute for shaders
	Renderer::VertexBufferAttributePtr TextureCoordAttribute = Renderer::VertexBufferAttributePtr(new Renderer::VertexBufferAttribute(TextureCoordBuffer, Renderer::ComponentDatatype::Float, 2));

	// create VertexArray and connect buffer with location in shader
	m_sceneVA = m_contextOGL->VCreateVertexArray();
	m_sceneVA->VSetIndexBuffer(indexBuffer);
	m_sceneVA->VSetAttribute(m_GroundShaderProgram->VGetVertexAttribute("in_Position"), PositionAttribute);
	m_sceneVA->VSetAttribute(m_GroundShaderProgram->VGetVertexAttribute("in_TexCoord"), TextureCoordAttribute);

	m_textureSampler = m_device->VCreateTexture2DSampler(Renderer::TextureMinificationFilter::Linear, Renderer::TextureMagnificationFilter::Nearest, Renderer::TextureWrap::Clamp, Renderer::TextureWrap::Clamp);

	///////////////////////////////////////////////////////////////////
	// RenderState

	// Render frontface of globe sphere
	m_renderStateAtmosphere.FaceCulling.FrontFaceWindingOrder = Renderer::WindingOrder::Counterclockwise;
	m_renderStateGlobe.FaceCulling.Enabled = true;
	m_renderStateGlobe.DepthTest.Enabled = true;

	// Render backface of Atmo'sphere' around the globe
	m_renderStateAtmosphere.FaceCulling.FrontFaceWindingOrder = Renderer::WindingOrder::Clockwise;
	m_renderStateAtmosphere.FaceCulling.Enabled = true;
	m_renderStateAtmosphere.DepthTest.Enabled = true;

	m_clearState.Color = Core::ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);

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
	// Camera
	CameraPosition = Core::Vector3<float>(m_sceneMesh->m_Center.x + 3.0f, m_sceneMesh->m_Center.y - 1, m_sceneMesh->m_Center.z);
	Core::Vector3<float> LookAt = Core::Vector3<float>(0.0f, 0.0f, 0.0f);
	Core::Vector3<float> UpVector = Core::Vector3<float>(0.0f, 1.0f, 0.0f);
	m_camera = new Renderer::Camera(CameraPosition, LookAt, UpVector, m_window->VGetWidth(), m_window->VGetHeight());

	PrepareShader();

	return true;
}

void GameShell::PrepareShader()
{
	Core::Vector3<float> LightPosition;
	Core::Vector3<float> LightDirection;
	Core::Vector3<float> InvWavelength;

	// Atmosphere Univorms
	int nSamples;
	float Kr, Kr4PI;
	float Km, Km4PI;
	float ESun;
	float g;
	float fExposure;

	float fInnerRadius;
	float fOuterRadius;
	float fScale;
	float fWavelength[3];
	float fWavelength4[3];
	float fRayleighScaleDepth;
	float fMieScaleDepth;

	// ============================================================================
	// Atmosphere Variables
	// ============================================================================

	nSamples = 3;		// Number of sample rays to use in integral equation

	Kr = 0.00025f;	// Rayleigh scattering constant
	Km = 0.0010f;		// Mie scattering constant
	ESun = 7.0f;		// Sun brightness constant

	float fKrESun = Kr*ESun;
	float fKmESun = Km*ESun;

	Kr4PI = Kr * 4.0f * Pi;
	Km4PI = Km * 4.0f *Pi;

	g = -0.990f;		// The Mie phase asymmetry factor
	float gSqr = g*g;

	fExposure = 2.0f;

	fWavelength[0] = 0.650f;		// 650 nm for red
	fWavelength[1] = 0.570f;		// 570 nm for green
	fWavelength[2] = 0.475f;		// 475 nm for blue
	fWavelength4[0] = 1.0f / powf(fWavelength[0], 4.0f);
	fWavelength4[1] = 1.0f / powf(fWavelength[1], 4.0f);
	fWavelength4[2] = 1.0f / powf(fWavelength[2], 4.0f);
	InvWavelength.Set(1 / fWavelength4[0], 1 / fWavelength4[1], 1 / fWavelength4[2]);

	fMieScaleDepth = 0.1f;

	fInnerRadius = 100.0f;
	fOuterRadius = fInnerRadius * 1.025f;
	fScale = 1 / (fOuterRadius - fInnerRadius);

	float fInnerRadiusSqr = fInnerRadius*fInnerRadius;
	float fOuterRadiusSqr = fOuterRadius*fOuterRadius;
	globeWorldMat.Scale(fInnerRadius);
	skyWorldMat.Scale(fOuterRadius);

	fRayleighScaleDepth = 0.25f;
	float fScaleOverScaleDepth = (1.0f / (fOuterRadius - fInnerRadius)) / fRayleighScaleDepth;

	LightPosition.Set(-20000.0f, 10000.0f, 0.0f);
	LightDirection = LightPosition / LightPosition.Length();

	///////////////////////////////////////////////////////////////////
	// Ground
	m_GroundShaderProgram->VSetUniform("diffuseTex", g_diffuseTextureID);

	m_GroundShaderProgram->VSetUniform("v3LightPos", LightPosition);
	m_GroundShaderProgram->VSetUniform("v3LightDir", LightDirection);
	m_GroundShaderProgram->VSetUniform("v3InvWavelength", InvWavelength);

	m_GroundShaderProgram->VSetUniform("fOuterRadius", fOuterRadius);
	m_GroundShaderProgram->VSetUniform("fOuterRadius2", fOuterRadiusSqr);
	m_GroundShaderProgram->VSetUniform("fInnerRadius", fInnerRadius);
	m_GroundShaderProgram->VSetUniform("fInnerRadius2", fInnerRadiusSqr);
	m_GroundShaderProgram->VSetUniform("fScaleOverScaleDepth", fScaleOverScaleDepth);
	m_GroundShaderProgram->VSetUniform("fScale", fScale);

	m_GroundShaderProgram->VSetUniform("fKrESun", fKrESun);
	m_GroundShaderProgram->VSetUniform("fKmESun", fKmESun);
	m_GroundShaderProgram->VSetUniform("fKr4PI", Kr4PI);
	m_GroundShaderProgram->VSetUniform("fKm4PI", Km4PI);

	///////////////////////////////////////////////////////////////////
	// Sky
	m_SkyShaderProgram->VSetUniform("v3LightPos", LightPosition);
	m_SkyShaderProgram->VSetUniform("v3LightDir", LightDirection);
	m_SkyShaderProgram->VSetUniform("v3InvWavelength", InvWavelength);

	m_SkyShaderProgram->VSetUniform("fOuterRadius", fOuterRadius);
	m_SkyShaderProgram->VSetUniform("fOuterRadius2", fOuterRadiusSqr);
	m_SkyShaderProgram->VSetUniform("fInnerRadius", fInnerRadius);
	m_SkyShaderProgram->VSetUniform("fInnerRadius2", fInnerRadiusSqr);
	m_SkyShaderProgram->VSetUniform("fScaleOverScaleDepth", fScaleOverScaleDepth);
	m_SkyShaderProgram->VSetUniform("fScale", fScale);

	m_SkyShaderProgram->VSetUniform("fKrESun", fKrESun);
	m_SkyShaderProgram->VSetUniform("fKmESun", fKmESun);
	m_SkyShaderProgram->VSetUniform("fKr4PI", Kr4PI);
	m_SkyShaderProgram->VSetUniform("fKm4PI", Km4PI);
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
	m_keyboard->VUpdate();
	m_mouse->VUpdate();

	// no input handling

	m_lastCursorPosition = m_mouse->VGetAbsolutePosition();
	globeWorldMat.RotateZ(0.0001f * deltaTime);
	skyWorldMat.RotateZ(0.0001f * deltaTime);
}


void GameShell::Render(void)
{
	m_contextOGL->VClear(m_clearState);
	m_contextOGL->VSetViewport(Renderer::Viewport(0, 0, m_window->VGetWidth(), m_window->VGetHeight()));
	m_contextOGL->VSetTextureSampler(g_diffuseTextureID, m_textureSampler);

	m_camera->SetResolution(m_window->VGetWidth(), m_window->VGetHeight());

	Core::Vector3<float> Position = globeWorldMat * CameraPosition;
	m_camera->SetViewLookAt(Position, Core::Vector3<float>(0.0f, 0.0f, 0.0f), Core::Vector3<float>(0.0f, 0.0f, 1.0f));

	float fCameraHeight = Position.Length();
	float fCameraHeightSqr = fCameraHeight * fCameraHeight;

	m_GroundShaderProgram->VSetUniform("ModelMatrix", (Core::Matrix3D<float>)globeWorldMat);
	m_GroundShaderProgram->VSetUniform("ViewMatrix", (Core::Matrix3D<float>)m_camera->CalculateView());
	m_GroundShaderProgram->VSetUniform("ProjectionMatrix", (Core::Matrix4x4<float>)m_camera->CalculateProjection());
	m_GroundShaderProgram->VSetUniform("fCameraHeight", fCameraHeight);
	m_GroundShaderProgram->VSetUniform("fCameraHeight2", fCameraHeightSqr);
	m_GroundShaderProgram->VSetUniform("v3CameraPos", Position);

	m_SkyShaderProgram->VSetUniform("ModelMatrix", (Core::Matrix3D<float>)skyWorldMat);
	m_SkyShaderProgram->VSetUniform("ViewMatrix", (Core::Matrix3D<float>)m_camera->CalculateView());
	m_SkyShaderProgram->VSetUniform("ProjectionMatrix", (Core::Matrix4x4<float>)m_camera->CalculateProjection());
	m_SkyShaderProgram->VSetUniform("fCameraHeight", fCameraHeight);
	m_SkyShaderProgram->VSetUniform("fCameraHeight2", fCameraHeightSqr);
	m_SkyShaderProgram->VSetUniform("v3CameraPos", Position);

	// Render Globe
	for (unsigned int i = 0; i < m_sceneMesh->GetNumberOfSubmeshes(); ++i)
	{
		m_contextOGL->VSetTexture(g_diffuseTextureID, m_diffuse_texture);
		m_contextOGL->VDraw(Renderer::PrimitiveType::Triangles, m_sceneMesh->m_SubMeshes[i].StartIndex, m_sceneMesh->m_SubMeshes[i].NumIndices, m_sceneVA, m_GroundShaderProgram, m_renderStateGlobe);
	}

	// Render Sky
	for (unsigned int i = 0; i < m_sceneMesh->GetNumberOfSubmeshes(); ++i)
	{
		m_contextOGL->VDraw(Renderer::PrimitiveType::Triangles, m_sceneMesh->m_SubMeshes[i].StartIndex, m_sceneMesh->m_SubMeshes[i].NumIndices, m_sceneVA, m_SkyShaderProgram, m_renderStateAtmosphere);
	}

	m_contextOGL->VSwapBuffers(true);
	m_window->VPollWindowEvents();
}
