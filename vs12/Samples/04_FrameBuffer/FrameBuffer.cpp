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
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(256, 256, "Framebuffer Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr ContextOGL = WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));

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
	VertexBufferAttributePtr PositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float) * 9), ComponentDatatype::Float, 3));
	PositionAttribute->GetVertexBuffer()->VCopyFromSystemMemory(vert, 0, sizeof(float) * 9);
	delete[] vert;

	float* texcoor = new float[6];
	texcoor[0] = 0.0f; texcoor[1] = 1.0f;
	texcoor[2] = 0.5f; texcoor[3] = 0.0f;
	texcoor[4] = 1.0f; texcoor[5] = 1.0f;

	VertexBufferAttributePtr TextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float) * 6), ComponentDatatype::Float, 2));
	TextureCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(texcoor, 0, sizeof(float) * 6);
	delete[] texcoor;

	// create VertexArray and connect buffer with location
	VertexArrayPtr VertexArray = ContextOGL->VCreateVertexArray();
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, PositionAttribute);
	VertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute);

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
	auto QuadPositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float)* 8), ComponentDatatype::Float, 2));
	QuadPositionAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadVert, 0, sizeof(float) * 8);
	delete[] QuadVert;

	auto QuadTextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float)* 8), ComponentDatatype::Float, 2));
	QuadTextureCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadTexcoor, 0, sizeof(float) * 8);
	delete[] QuadTexcoor;

	// connect buffer with location
	auto QuadVertexArray = ContextOGL->VCreateVertexArray();
	QuadVertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, QuadPositionAttribute);
	QuadVertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord")->Location, QuadTextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Textures

	Texture2DPtr texture = DeviceOGL->VCreateTexture2DFromFile("../Data/Textures/test.jpg");
	TextureSamplerPtr sampler = DeviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Repeat, TextureWrap::Repeat);

	///////////////////////////////////////////////////////////////////
	// FrameBuffer

	FramebufferPtr FrameBuffer = ContextOGL->VCreateFramebuffer();

	Viewport viewport = ContextOGL->VGetViewport();
	int out_Color_Location = ShaderProgram->VGetFragmentOutputLocation("out_Color");

	Texture2DPtr renderTarget = DeviceOGL->VCreateTexture2D(Texture2DDescription(viewport.width, viewport.height, TextureFormat::RedGreenBlue8));
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
	ContextOGL->VSetTextureSampler(diffuseTex, sampler);
	ShaderProgram->VSetUniform("diffuseTex", diffuseTex);

	while (!WindowOGL->VShouldClose())
	{
		// Render Triangle to Framebuffer
		ContextOGL->VSetFramebuffer(FrameBuffer);

		ContextOGL->VClear(clearBlue);
		ContextOGL->VSetTexture(diffuseTex, texture);
		ContextOGL->VSetViewport(Viewport(0, 0, viewport.width, viewport.height));
		ContextOGL->VDraw(PrimitiveType::Triangles, VertexArray, ShaderProgram, renderState);

		// Render Fullscreen Quad to Backbuffer
		ContextOGL->VSetFramebuffer(nullptr);

		ContextOGL->VClear(clearBlack);
		ContextOGL->VSetTexture(diffuseTex, FrameBuffer->VGetColorAttachment(out_Color_Location));
		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));
		ContextOGL->VDraw(PrimitiveType::TriangleStrip, QuadVertexArray, ShaderProgram, renderState);

		ContextOGL->VSwapBuffers();
		WindowOGL->VPollWindowEvents();

		std::shared_ptr<void> image = renderTarget->VCopyToSystemMemory(ImageFormat::BlueGreenRed, ImageDatatype::UnsignedByte);
		Bitmap bmp;
		bmp.SaveFile(image.get(), WindowOGL->VGetWidth(), WindowOGL->VGetHeight(), "out.bmp");

		return 0;
	}
	return 0;
}