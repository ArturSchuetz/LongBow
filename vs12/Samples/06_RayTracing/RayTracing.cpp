#include "BowRenderer.h"

#include <cstdint>
#include <windows.h>

#include "resource.h"

#include "Scene.h"
#include "Raytracer.h"
#include "Sphere.h"
#include "Triangle.h"

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

// Bildaufloesung
#define PIC_WIDTH 512
#define PIC_HEIGHT 512

int lineNum = 0;			// aktuelle Zeile

Raytracer myRaytracer;		// Raytracer Objekt
unsigned char *pixels;		// Pixel

int main()
{
	// Creating Render Device
	RenderDevicePtr DeviceOGL = RenderDeviceManager::GetInstance().GetOrCreateDevice(API::OpenGL3x);
	if (DeviceOGL == nullptr)
	{
		return 0;
	}

	// Creating Window
	GraphicsWindowPtr WindowOGL = DeviceOGL->VCreateWindow(PIC_WIDTH, PIC_HEIGHT + 10, "Ray Tracing Sample", WindowType::Windowed);
	if (WindowOGL == nullptr)
	{
		return 0;
	}

	RenderContextPtr ContextOGL = WindowOGL->VGetContext();
	ShaderProgramPtr ShaderProgram = DeviceOGL->VCreateShaderProgram(LoadShaderFromResouce(IDS_VERTEXSHADER), LoadShaderFromResouce(IDS_FRAGMENTSHADER));
	
	if (ShaderProgram == nullptr)
	{
		return 0;
	}

	///////////////////////////////////////////////////////////////////
	// Initialize raytracer and create scene

	pixels = new unsigned char[PIC_WIDTH * PIC_HEIGHT * 3];

	Scene myScene = Scene(PIC_WIDTH, PIC_HEIGHT);	// Szene anlegen

	// Materialien hinzufuegen (Farbe, Diffus, Spekular, Phong Exponent)
	myScene.addMaterial(new Material(Vector3<float>(1.0f, 0.2f, 0.2f), 0.8f, 0.2f, 10.0f));
	myScene.addMaterial(new Material(Vector3<float>(0.1f, 0.9f, 0.1f), 0.5f, 0.5f, 50.0f));
	myScene.addMaterial(new Material(Vector3<float>(0.1f, 0.1f, 0.9f), 0.8f, 0.2f, 100.0f));
	myScene.addMaterial(new Material(Vector3<float>(1.0f, 1.0f, 0.2f), 0.8f, 0.2f, 20.0f));
	myScene.addMaterial(new Material(Vector3<float>(0.0f, 1.0f, 0.8f), 0.8f, 0.2f, 30.0f));

	// Punktlichtquellen hinzufuegen (Position, Farbe, Lichtstaerke)
	myScene.addLight(new Lightsource(Vector3<float>(4.0f, 6.0f, 2.0f), Vector3<float>(1.0f, 1.0f, 1.0f), 5));
	myScene.addLight(new Lightsource(Vector3<float>(-4.0f, 3.0f, 1.0f), Vector3<float>(1.0f, 1.0f, 1.0f), 5));

	// Objekte hinzufuegen
	myScene.addObject(new SphereObject(Sphere<float>(Vector3<float>(1.1f, 1.1f, 1.1f), 1.0f), 3));
	myScene.addObject(new SphereObject(Sphere<float>(Vector3<float>(-1.1f, 1.1f, 1.1f), 1.0f), 0));
	myScene.addObject(new SphereObject(Sphere<float>(Vector3<float>(0.0f, 1.1f, -1.1f), 1.0f), 2));
	myScene.addObject(new SphereObject(Sphere<float>(Vector3<float>(0.0f, 2.0f, 0.0f), 1.0f), 4));
	myScene.addObject(new TriangleObject(Triangle<float>(Vector3<float>(-5.0f, 0.0f, 5.0f), Vector3<float>(5.0f, 0.0f, 5.0f), Vector3<float>(5.0f, 0.0f, -5.0f)), 1));
	myScene.addObject(new TriangleObject(Triangle<float>(Vector3<float>(-5.0f, 0.0f, 5.0f), Vector3<float>(5.0f, 0.0f, -5.0f), Vector3<float>(-5.0f, 0.0f, -5.0f)), 1));

	myRaytracer.init(5, &myScene, pixels);		// init ray tracer mit 5 Reflexionen

	///////////////////////////////////////////////////////////////////
	// Create clear state and color

	ClearState clearState;
	clearState.Color = ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);

	///////////////////////////////////////////////////////////////////
	// Create RenderState

	RenderState renderState;
	renderState.FaceCulling.Enabled = false;
	renderState.DepthTest.Enabled = false;

	///////////////////////////////////////////////////////////////////
	// Screenfilling Quad Preperations

	Vector2<float>* QuadVert = new Vector2<float>[4];     // vertex array 
	QuadVert[0].x = -1.0f; QuadVert[0].y = 1.0f;
	QuadVert[1].x = -1.0f; QuadVert[1].y = -1.0f;
	QuadVert[2].x = 1.0f; QuadVert[2].y = 1.0f;
	QuadVert[3].x = 1.0f; QuadVert[3].y = -1.0f;

	Vector2<float>* QuadTexcoor = new Vector2<float>[4];     // vertex array 
	QuadTexcoor[0].x = 0.0f; QuadTexcoor[0].y = 1.0f;
	QuadTexcoor[1].x = 0.0f; QuadTexcoor[1].y = 0.0f;
	QuadTexcoor[2].x = 1.0f; QuadTexcoor[2].y = 1.0f;
	QuadTexcoor[3].x = 1.0f; QuadTexcoor[3].y = 0.0f;

	// fill buffer with informations
	auto QuadPositionAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float) * 8), ComponentDatatype::Float, 2));
	QuadPositionAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadVert, 0, sizeof(float) * 8);
	delete[] QuadVert;

	auto QuadTextureCoordAttribute = VertexBufferAttributePtr(new VertexBufferAttribute(DeviceOGL->VCreateVertexBuffer(BufferHint::StaticDraw, sizeof(float) * 8), ComponentDatatype::Float, 2));
	QuadTextureCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(QuadTexcoor, 0, sizeof(float) * 8);
	delete[] QuadTexcoor;

	// connect buffer with location
	auto QuadVertexArray = ContextOGL->VCreateVertexArray();
	QuadVertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_Position")->Location, QuadPositionAttribute);
	QuadVertexArray->VSetAttribute(ShaderProgram->VGetVertexAttribute("in_TexCoord")->Location, QuadTextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Create Texture to Render with

	Texture2DPtr RayTracerRenderTarget = DeviceOGL->VCreateTexture2D(Texture2DDescription(PIC_WIDTH, PIC_HEIGHT, TextureFormat::RedGreenBlue8, false));
	TextureSamplerPtr sampler = DeviceOGL->VCreateTexture2DSampler(TextureMinificationFilter::Linear, TextureMagnificationFilter::Linear, TextureWrap::Repeat, TextureWrap::Repeat);
	
	int texLocation = 0;
	ContextOGL->VSetTextureSampler(texLocation, sampler);
	ShaderProgram->VSetUniform("diffuseTex", texLocation);

	///////////////////////////////////////////////////////////////////
	// Gameloop

	while (!WindowOGL->VShouldClose())
	{
		ContextOGL->VClear(clearState);

		if (lineNum < PIC_HEIGHT) {
			myRaytracer.renderLine(lineNum);						// eine Zeile berechnen
			lineNum++;												// naechste Zeile 
			myRaytracer.writePPM(&myScene, "out.ppm", &pixels[0]);	// Bild speichern
			RayTracerRenderTarget->VCopyFromSystemMemory(&pixels[0], PIC_WIDTH, PIC_HEIGHT, ImageFormat::RedGreenBlue, ImageDatatype::UnsignedByte);
		}
		ContextOGL->VSetTexture(texLocation, RayTracerRenderTarget);

		ContextOGL->VSetViewport(Viewport(0, 0, WindowOGL->VGetWidth(), WindowOGL->VGetHeight()));

		ContextOGL->VDraw(PrimitiveType::TriangleStrip, QuadVertexArray, ShaderProgram, renderState);

		ContextOGL->VSwapBuffers();
	}

	delete[] pixels;
	return 0;
}
