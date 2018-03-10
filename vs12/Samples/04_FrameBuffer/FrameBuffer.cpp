#include "BowRenderer.h"
#include "BowResources.h"

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
	// Creating Render Device
	RenderDevicePtr deviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(RenderDeviceAPI::OpenGL3x);
	if (deviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr windowOGL = deviceOGL->VCreateWindow(256, 256, "Framebuffer Sample", WindowType::Windowed);
	if (windowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr contextOGL = windowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = deviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	ClearState clearBlue;
	clearBlue.Color = ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

	ClearState clearBlack;
	clearBlack.Color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	float* vert = new float[9];
	vert[0] = -1.0f; vert[1] = 1.0f; vert[2] = 1.0f;
	vert[3] = 0.0f; vert[4] = -1.0f; vert[5] = 1.0f;
	vert[6] = 1.0f; vert[7] = 1.0f; vert[8] = 1.0f;

	// fill buffer with informations
	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float) * 9), ComponentDatatype::Float, 3));
	PositionAttribute->GetVertexBuffer()->VCopyFromSystemMemory(vert, 0, sizeof(float) * 9);
	delete[] vert;

	float* texcoor = new float[6];
	texcoor[0] = 0.0f; texcoor[1] = 1.0f;
	texcoor[2] = 0.5f; texcoor[3] = 0.0f;
	texcoor[4] = 1.0f; texcoor[5] = 1.0f;

	VertexBufferAttributePtr TextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float) * 6), ComponentDatatype::Float, 2));
	TextureCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(texcoor, 0, sizeof(float) * 6);
	delete[] texcoor;

	// create VertexArray and connect buffer with location
	VertexArrayPtr VertexArray = contextOGL->VCreateVertexArray();
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position"), PositionAttribute);
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord"), TextureCoordAttribute);

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
	auto QuadPositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float)* 8), ComponentDatatype::Float, 2));
	QuadPositionAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadVert, 0, sizeof(float) * 8);
	delete[] QuadVert;

	auto QuadTextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(deviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float)* 8), ComponentDatatype::Float, 2));
	QuadTextureCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadTexcoor, 0, sizeof(float) * 8);
	delete[] QuadTexcoor;

	// connect buffer with location
	auto QuadVertexArray = contextOGL->VCreateVertexArray();
	QuadVertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position"), QuadPositionAttribute);
	QuadVertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord"), QuadTextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture;

	ImagePtr image = ImageManager::GetInstance().Load("../Data/Textures/test.jpg");
	if (image->GetSizeInBytes() / (image->GetHeight() * image->GetWidth()) == 3)
		texture = deviceOGL->VCreateTexture2D(image, TextureFormat::RedGreenBlue8);
	else
		texture = deviceOGL->VCreateTexture2D(image, TextureFormat::RedGreenBlueAlpha8);

	TextureSamplerPtr sampler = deviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Repeat, TextureWrap::Repeat);

	///////////////////////////////////////////////////////////////////
	// FrameBuffer

	FramebufferPtr FrameBuffer = contextOGL->VCreateFramebuffer();

	Viewport viewport = contextOGL->VGetViewport();
	int out_Color_Location = ShaderProgram->VGetFragmentOutputLocation("out_Color");

	Texture2DPtr renderTarget = deviceOGL->VCreateTexture2D(Texture2DDescription(viewport.width, viewport.height, TextureFormat::RedGreenBlue8));
	FrameBuffer->VSetColorAttachment(out_Color_Location, renderTarget);

	///////////////////////////////////////////////////////////////////
	// RenderState

	RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.DepthTest.Enabled = false;
	renderState.StencilTest.Enabled = false;

	///////////////////////////////////////////////////////////////////
	// Gameloop

	int diffuseTex = 0;
	contextOGL->VSetTextureSampler(diffuseTex, sampler);
	ShaderProgram->VSetUniform("diffuseTex", diffuseTex);

	while (!windowOGL->VShouldClose())
	{
		// Render Triangle to Framebuffer
		contextOGL->VSetFramebuffer(FrameBuffer);

		contextOGL->VClear(clearBlue);
		contextOGL->VSetTexture(diffuseTex, texture);
		contextOGL->VSetViewport(Viewport(0, 0, viewport.width, viewport.height));
		contextOGL->VDraw(PrimitiveType::Triangles, VertexArray, ShaderProgram, renderState);

		// Render Fullscreen Quad to Backbuffer
		contextOGL->VSetFramebuffer(nullptr);

		contextOGL->VClear(clearBlack);
		contextOGL->VSetTexture(diffuseTex, FrameBuffer->VGetColorAttachment(out_Color_Location));
		contextOGL->VSetViewport(Viewport(0, 0, windowOGL->VGetWidth(), windowOGL->VGetHeight()));
		contextOGL->VDraw(PrimitiveType::TriangleStrip, QuadVertexArray, ShaderProgram, renderState);

		contextOGL->VSwapBuffers();
		windowOGL->VPollWindowEvents();
	}
	return 0;
}